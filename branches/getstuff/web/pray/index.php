<?php
session_start();
header("Cache-Control: no-cache, must-revalidate");
header("Pragma: no-cache");
header("Content-type: text/html; charset=utf-8");
?>
<head>
	<meta name="viewport" content="user-scalable=no, width=device-width" />  
	<title>代祷本</title>
	<style type="text/css">
	pre
	{
		margin: 10px 0 0 0;
		padding: 10px;
		border: 0;
		border: 1px dotted #785;
		background: #f5f5f5;
		font-family: "Courier New",monospace;
		font-size: 12px;
	}
	pre p
	{
		text-indent: 1em;
		font-family: "Microsoft JhengHei",SimSun,monospace;
		color:#000000;
		font-size: 15px;
		font-weight:bold;
	}
	pre span
	{
		color:#999;
	}
	
	input[type=submit]
	{
		margin: 20px 0 0 20%;
		background-position: bottom left;
		width: 60%;
		height: 30px;
		background-color:#f5f5f;
	}
	</style>
</head>
<html>
<?php
	//先从数据库中获取
	$conn = mysql_pconnect("localhost","liyake","me_lyk");
	if($conn)
	{
		mysql_select_db("liyake",$conn);
		$result = mysql_query("select name,text,createtime from pray order by id desc limit 10;");
		while ($row = mysql_fetch_array($result))
		{
			echo('<pre><span  style="width:100%">昵称：'.$row['name'].'  留言时间：'.date('Y-m-d H:i',strtotime($row['createtime'])+3600*8).'</span><p>'.$row['text'].'</p></pre>');
		}
	}
?>
<hr/>
<p><strong>提交你的代祷意向：</strong></p>
<form action="update.php" method="post">
	<label for="input_name" style="width:100px;">昵称：</label><input name="name" id="input_name" type="text" value="<?php
		if(isset($_SESSION['name']))
		{echo $_SESSION['name'];}else{ echo '匿名';}
		?>"></input>
	<br/>
	<p>代祷内容：</p>
	<textarea id="input_text" name="text" style="width:100%; height:80px">在此输入你的祈祷意向，然后点击提交</textarea><br/>
	<input type="submit" value=" 提 交 ">
</form>

	<script type="text/javascript">
	var t = document.getElementById('input_text');
	t.onfocus = function(){
		if(this.innerHTML == '在此输入你的祈祷意向，然后点击提交'){this.innerHTML = '';}
	};

	t.onblur = function(){
		if(this.innerHTML == ''){
			this.innerHTML = '在此输入你的祈祷意向，然后点击提交';
		}
	};
	</script>
</html>