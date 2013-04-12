<?php
session_start();
header("Content-type: text/html; charset=utf-8");
?>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<title>update</title>
<script type="text/javascript" src="scripts/jquery-1.3.2.min.js"></script>
<script type="text/javascript" src="scripts/jHtmlArea-0.7.5.js"></script>
<script type="text/javascript" src="WdatePicker/WdatePicker.js"></script>
<link rel="Stylesheet" type="text/css" href="style/jHtmlArea.css" />	
<script type="text/javascript">
function refreshOK(obj)
{
	var myObj = JSON.parse(obj);
	if(myObj["mass"]=="")
		myObj["mass"]="<p></p>";
	if(myObj["med"]=="")
		myObj["med"]="<p></p>";
	if(myObj["comp"]=="")
		myObj["comp"]="<p></p>";
	if(myObj["let"]=="")
		myObj["let"]="<p></p>";
	if(myObj["lod"]=="")
		myObj["lod"]="<p></p>";
	if(myObj["thought"]=="")
		myObj["thought"]="<p></p>";
	if(myObj["ordo"]=="")
		myObj["ordo"]="<p></p>";
	if(myObj["ves"]=="")
		myObj["ves"]="<p></p>";
	if(myObj["saint"]=="")
		myObj["saint"]="<p></p>";
	$('#input_mass').htmlarea('html',myObj["mass"]);
	$('#input_med').htmlarea('html',myObj["med"]);
	$('#input_comp').htmlarea('html',myObj["comp"]);
	$('#input_let').htmlarea('html',myObj["let"]);
	$('#input_lod').htmlarea('html',myObj["lod"]);
	$('#input_thought').htmlarea('html',myObj["thought"]);
	$('#input_ordo').htmlarea('html',myObj["ordo"]);
	$('#input_ves').htmlarea('html',myObj["ves"]);
	$('#input_saint').htmlarea('html',myObj["saint"]);
//	alert($('#input_saint').htmlarea('html'));
	input_date.disabled = false;
}

function refreshPage()
{
	input_date.disabled = true;
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			refreshOK(xmlhttp.responseText);
		}
	}
	xmlhttp.open("GET","../getstuff/getstuff.php?date="+input_date.value,true);
	xmlhttp.send();
}

function OnDateSelected()
{
	$('#input_mass').focus();
	try
	{
		if(lastvalue != input_date.value)
		{
			lastvalue = input_date.value;
			refreshPage();
		}
	}
	catch(err)
	{
		lastvalue = input_date.value;
		refreshPage();
	}
}

function onlogin()
{
	var user = username.value;
	var pass = password.value;
	
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			alert(xmlhttp.responseText);
			parent.location.reload();
			return;
		}
	}
	xmlhttp.open("POST","../users/login.php",true);
	xmlhttp.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
	xmlhttp.send("submit=1&username="+user+"&password="+pass+"");
}

function onlogout()
{
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			parent.location.reload();
			return;
		}
	}
	xmlhttp.open("POST","../users/logout.php",true);
	xmlhttp.send();
}

function checkString(obj)
{
	if(obj=="")
		return true;
	var strpattern = /^[^\"\'\{\}]+$/;
	if(!strpattern.test(obj))
	{
		alert(obj);
		return false;
	}
	return true;
}

function isAllInputValid()
{
	if(!checkString($('#input_mass').htmlarea('html')))
		return false;
	if(!checkString($('#input_med').htmlarea('html')))
		return false;
	if(!checkString($('#input_comp').htmlarea('html')))
		return false;
	if(!checkString($('#input_let').htmlarea('html')))
		return false;
	if(!checkString($('#input_lod').htmlarea('html')))
		return false;
	if(!checkString($('#input_thought').htmlarea('html')))
		return false;
	if(!checkString($('#input_ordo').htmlarea('html')))
		return false;
	if(!checkString($('#input_ves').htmlarea('html')))
		return false;
	if(!checkString($('#input_saint').htmlarea('html')))
		return false;
	
	return true;
}

function getinputvalue(obj)
{
	var post_Str = obj.replace(/\+/g, "%2B");//"+"转义    
	var post_Str= post_Str.replace(/\&/g, "%26");//"&"  
	var post_Str= post_Str.replace(/\#/g, "%23");//"#"
	return post_Str;
}

function onupdate()
{
	if(!isAllInputValid())
	{
		alert("文字中不允许出现特殊字符： \" \' { }");
		return;
	}
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			alert(xmlhttp.responseText);
			$("#btnUpdate")[0].disabled = false;
			return;
		}
	}
	
	$("#btnUpdate")[0].disabled = true;
	
	xmlhttp.open("POST","../getstuff/update.php",true);
	xmlhttp.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
	var params = "date="+input_date.value+"&mass="+getinputvalue($('#input_mass').htmlarea('html'));
	params+="&med="+getinputvalue($('#input_med').htmlarea('html'));
	params+="&comp="+getinputvalue($('#input_comp').htmlarea('html'));
	params+="&let="+getinputvalue($('#input_let').htmlarea('html'));
	params+="&lod="+getinputvalue($('#input_lod').htmlarea('html'));
	params+="&thought="+getinputvalue($('#input_thought').htmlarea('html'));
	params+="&ordo="+getinputvalue($('#input_ordo').htmlarea('html'));
	params+="&ves="+getinputvalue($('#input_ves').htmlarea('html'));
	params+="&saint="+getinputvalue($('#input_saint').htmlarea('html'));
	xmlhttp.send(params);
}

$(function() {
//	$("#test").htmlarea();

	$("textarea").htmlarea({
	toolbar: ["html", "italic", "underline", "|", "h1", "h2", "h3", "h4", "h5", "h6", "p"
		], // Overrides/Specifies the Toolbar buttons to show
	});
	
//	$("textarea").htmlarea(); // Initialize jHtmlArea's with all default values
});
</script>
    <style type="text/css">
        /* body { background: #ccc;} */
        div.jHtmlArea .ToolBar ul li a.custom_disk_button 
        {
            background: url(images/disk.png) no-repeat;
            background-position: 0 0;
        }
        
        div.jHtmlArea { border: solid 1px #ccc; }
    </style>
    <style type="text/css">
        .TextArea1
        {
            width: 100%;
			height: 300px;
            margin-top: 0px;
        }
        .style1
        {
            width: 50px;
        }
        .style2
        {
            width: 50%;
            text-align: right;
        }
    </style>
</head>
<!--
	$stuff_mass = "";		//弥撒
	$stuff_med = "";		//日祷
	$stuff_comp = "";		//夜祷
	$stuff_let = "";		//诵读
	$stuff_lod = "";		//晨祷
	$stuff_thought = "";	//反省
	$stuff_ordo = "";		//礼仪
	$stuff_ves = "";		//晚祷
	$stuff_saint = "";		//圣人传记
!-->
<?php
//登录
if(!isset($_SESSION['userid'])){
    echo '
<div>
<label for="username" class="label">用户名:</label>
<input id="username" name="username" type="text"/>
<label for="password" class="label">密 码:</label>
<input id="password" name="password" type="password"/>
<input type="button" name="btnLogin" value="  确 定  " onclick="onlogin()"/>
</div>';
}
else
{
	$username = $_SESSION['name'];
	echo "欢迎你，".$username;
echo '<input type="button" name="btnLogin" value=" 注 销 " onclick="onlogout()"/>';
echo '<input type="button" name="btnUpdate" id="btnUpdate" value=" 更 新 " onclick="onupdate()"/>';
}
?>
<table style="width: 100%; height: 20px;">
	<tr>
		<td class="style2">日期：</td>
		<td ><input id="input_date" type="text" style="width:70px" onclick="WdatePicker({onpicked:function(){this.onchange();}})" onchange="OnDateSelected()"></td>
	</tr>
</table>
<table style="width: 100%; height: 100%;">
    <tr>
        <td class="style1">弥撒:</td>
        <td><textarea class="TextArea1" id="input_mass"></textarea></td>
    </tr>
    <tr>
        <td class="style1">日祷:</td>
        <td><textarea class="TextArea1" id="input_med"></textarea></td>
    </tr>
    <tr>
        <td class="style1">夜祷:</td>
        <td><textarea class="TextArea1" id="input_comp"></textarea></td>
    </tr>
    <tr>
        <td class="style1">诵读:</td>
        <td><textarea class="TextArea1" id="input_let"></textarea></td>
    </tr>
    <tr>
        <td class="style1">晨祷:</td>
        <td><textarea class="TextArea1" id="input_lod"></textarea></td>
    </tr>
    <tr>
        <td class="style1">反省:</td>
        <td><textarea class="TextArea1" id="input_thought"></textarea></td>
    </tr>
    <tr>
        <td class="style1">礼仪:</td>
        <td><textarea class="TextArea1" id="input_ordo"></textarea></td>
    </tr>
    <tr>
        <td class="style1">晚祷:</td>
        <td><textarea class="TextArea1" id="input_ves"></textarea></td>
    </tr>
    <tr>
        <td class="style1">圣人传记:</td>
        <td><textarea class="TextArea1" id="input_saint"></textarea></td>
    </tr>
</table>
