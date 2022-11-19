@[TOC]( )

# [模型部署]：TVM模型部署实战
模型基于通用的深度学习框架开发，如TensorFlow，PyTorch等，导出相应的高级别计算图文件，```*.tf， *.pt or 更加通用的计算图框架 *.onnx```

## 1 PyTorch模型

### 1.1 准备模型
要编译PyTorch模型，首先是需要有一个名为.pt文件（TorchScript模型文件）。
>.pt文件和.pth文件的区别，见上篇[模型部署原理介绍](https://blog.csdn.net/qq_41897558/article/details/127925647?spm=1001.2014.3001.5502)
```python
import tvm
from tvm import relay

import numpy as np

from tvm.contrib.download import download_testdata

# 导入 PyTorch
import torch
import torchvision

# 加载预训练的PyTorch模型
# 下载resnet18-*.pth
model_name = "resnet18"
model = getattr(torchvision.models, model_name)(pretrained=True)
model = model.eval()

# 通过追踪获取 TorchScripted 模型
input_shape = [1, 3, 224, 224]
input_data = torch.randn(input_shape)
scripted_model = torch.jit.trace(model, input_data).eval()
```

### 1.2 准备数据
经典的猫咪事例：
```python
from PIL import Image

img_url = "https://github.com/dmlc/mxnet.js/blob/main/data/cat.png?raw=true"
img_path = download_testdata(img_url, "cat.png", module="data")
img = Image.open(img_path).resize((224, 224))

# 预处理图像，并将其转换为张量
from torchvision import transforms

my_preprocess = transforms.Compose(
    [
        transforms.Resize(256),
        transforms.CenterCrop(224),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
    ]
)
img = my_preprocess(img)
img = np.expand_dims(img, 0)

```

### 1.3 将TorchScript计算图导入TVM
将 PyTorch 计算图转换为 Relay 计算图。input_name 可以是任意值。

```python
input_name = "input0"
# like fake input
shape_list = [(input_name, img.shape)]
mod, params = relay.frontend.from_pytorch(scripted_model, shape_list)
```

### 1.4 使用Relay构建
用给定的输入规范，将计算图编译为 llvm target。
```python
target = tvm.target.Target("llvm", host="llvm")
dev = tvm.cpu(0)
with tvm.transform.PassContext(opt_level=3):
    lib = relay.build(mod, target=target, params=params)
```

### 1.5 将部署好的TVM模型 使用Python 加载
```python
from tvm.contrib import graph_executor

dtype = "float32"
m = graph_executor.GraphModule(lib["default"](dev))
# 设置输入
m.set_input(input_name, tvm.nd.array(img.astype(dtype)))
# 执行
m.run()
# 得到输出
tvm_output = m.get_output(0)
```

### 1.6 将编译好的TVM模型 使用C++ 加载
TVM可以导出什么呢？
编写代码时，仅能导出用tvm.relay.build构建的完整模型，TVM 将生成的库导出为动态共享对象（例如 DLL (Windows) 或 .so (linux)）。通过使用 libtvm_runtime.so 将它们加载到可执行文件中，可以使用这些库执行推理。

1. 导出 .so， 动态链接库。
```python
def prepare_graph_lib(mod, base_path):
    # build a module
    compiled_lib = relay.build(mod, tvm.target.Target("llvm"), params=params)
    # export it as a shared library
    # If you are running cross compilation, you can also consider export
    # to tar and invoke host compiler later.
    dylib_path = os.path.join(base_path, "test_relay_add.so")
    compiled_lib.export_library(dylib_path)
```
2. C++ 端侧加载动态链接库

```c++
#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

#include <cstdio>


void DeployGraphExecutor() {
  LOG(INFO) << "Running graph executor...";
  // load in the library
  DLDevice dev{kDLCPU, 0};
  tvm::runtime::Module mod_factory = tvm::runtime::Module::LoadFromFile("lib/test_relay_add.so");
  // create the graph executor module
  tvm::runtime::Module gmod = mod_factory.GetFunction("default")(dev);
  tvm::runtime::PackedFunc set_input = gmod.GetFunction("set_input");
  tvm::runtime::PackedFunc get_output = gmod.GetFunction("get_output");
  tvm::runtime::PackedFunc run = gmod.GetFunction("run");

  // Use the C++ API, Fake data
  tvm::runtime::NDArray x = tvm::runtime::NDArray::Empty({2, 2}, DLDataType{kDLFloat, 32, 1}, dev);
  tvm::runtime::NDArray y = tvm::runtime::NDArray::Empty({2, 2}, DLDataType{kDLFloat, 32, 1}, dev);

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      static_cast<float*>(x->data)[i * 2 + j] = i * 2 + j;
    }
  }
  // set the right input
  set_input("x", x);
  // run the code
  run();
  // get the output
  get_output(0, y);

  // Test add func
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      ICHECK_EQ(static_cast<float*>(y->data)[i * 2 + j], i * 2 + j + 1);
    }
  }
}

int main(void) {
  DeployGraphExecutor();
  return 0;
}
```

## 2. 模型量化
量化过程要找到每一层的每个权重和中间特征图 (feature map) 张量的 scale。

对于权重而言，scales 是根据权重的值直接计算出来的。支持两种模式：power2 和 max。这两种模式都是先找到权重张量内的最大值。在 power2 模式下，最大值向下舍入为 2 的幂。如果权重和中间特征图的 scale 都是 2 的幂，则可以利用移位 (bit shifting) 进行乘法运算，这使得计算效率更高。在 max 模式下，最大值用作 scale。如果不进行四舍五入，在某些情况下 max 模式可能具有更好的精度。当 scale 不是 2 的幂时，将使用定点乘法。

中间特征图可以通过数据感知量化来找到 scale。数据感知量化将校准数据集作为输入参数，通过最小化量化前后激活分布之间的 KL 散度来计算 scales。或者也可以用预定义的全局 scales，这样可以节省校准时间，但会影响准确性。

```python
def quantize(mod, params, data_aware):
    if data_aware:
        with relay.quantize.qconfig(calibrate_mode="kl_divergence", weight_scale="max"):
            mod = relay.quantize.quantize(mod, params, dataset=calibrate_dataset())
    else:
        with relay.quantize.qconfig(calibrate_mode="global_scale", global_scale=8.0):
            mod = relay.quantize.quantize(mod, params)
    return mod
```