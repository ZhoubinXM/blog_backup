

## 1. VSCode中为Python文件添加头部注释

实际编写Python文件时，往往需要为文件添加相关说明，例如文件名称、文件作用、创建时间、作者信息、版本号等等。这些信息往往是固定模板的，因此希望有一种方式可以自动的为我们添加上这些信息。下面介绍一种在VS Code中自动为python文件添加头部注释的方法。

依次单击菜单栏 “File”—“Preferences”—“User Snippets”，然后选择python后会生成python.json文件，将该文件内容替换为以下内容：

```text
{
	// Place your snippets for python here. Each snippet is defined under a snippet name and has a prefix, body and 
	// description. The prefix is what is used to trigger the snippet and the body will be expanded and inserted. Possible variables are:
	// $1, $2 for tab stops, $0 for the final cursor position, and ${1:label}, ${2:another} for placeholders. Placeholders with the 
	// same ids are connected.
	// Example:
	// "Print to console": {
	// 	"prefix": "log",
	// 	"body": [
	// 		"console.log('$1');",
	// 		"$2"
	// 	],
	// 	"description": "Log output to console"
	// }
		"HEADER": {
		  "prefix": "header",
		  "body": [
			"#!/usr/bin/env python",
			"# -*- encoding: utf-8 -*-",
			"'''",
			"@file    :$TM_FILENAME",
			"@data    :$CURRENT_YEAR/$CURRENT_MONTH/$CURRENT_DATE $CURRENT_HOUR:$CURRENT_MINUTE:$CURRENT_SECOND",
			"@author    :name (mail)",
			"@version    :1.0",
			"@brief    :",
			"'''",
			"",
			"$0"
		  ],
		}
}
```

保存修改即可。

新建一个python文件，然后在头部敲入header即可自动弹出注释。