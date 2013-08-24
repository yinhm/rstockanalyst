//指数提取
//http://quote.591hx.com/StockList.aspx?item=all_index&name=%u5206%u7c7b
var myContent = "";
var myT = document.getElementById('sortTable');
var myTB = myT.getElementsByTagName('tbody')[0];
for( var l in myTB.children)
{
	try
	{
		var a = myTB.children[l].children[0].textContent;
		if(a!="")
		{
			myContent = myContent + "\n" + a + "SZ";
		}
	}
	catch(err)
	{
	}
}


//债券提取
var myContent = "";
var myTB = document.getElementsByTagName('tbody')[0];
for( var l in myTB.children)
{
	try
	{
		var a = myTB.children[l].children[1].textContent.trim();
		if(a!="")
		{
			myContent = myContent + "\n" + a + "SZ";
		}
		var b = myTB.children[l].children[3].textContent.trim();
		if(b!="")
		{
			myContent = myContent + "\n" + b + "SH";
		}
	}
	catch(err)
	{
	}
}

//转债列表
//http://biz.sse.com.cn/sseportal/webapp/datapresent/ZQSearch1Act?reportName=BizCompZQSearchRpt&BOND_CODE=&BOND_NAME=&page_flag=2