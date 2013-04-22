<?php
//define your token
require_once("html2text.php");
define("TOKEN", "test");
//global $helpstr = "帮助列表：\n弥撒:可获取今日弥撒经文及读经\n日祷:可获取今日祷告用文\n圣人:获取圣人传记";

$wechatObj = new wechatCallbackapiTest();
if($wechatObj->checkSignature())
{
	$wechatObj->responseMsg();
}

class wechatCallbackapiTest
{
	public function valid()
    {
        $echoStr = $_GET["echostr"];

        //valid signature , option
        if($this->checkSignature())
		{
        	echo $echoStr;
        	exit;
        }
    }

    public function responseMsg()
    {
		//get post data, May be due to the different environments
		$postStr = $GLOBALS["HTTP_RAW_POST_DATA"];
      	//extract post data
		if (!empty($postStr))
		{
			$resultStr = "";
			
			$postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
			if($postObj->MsgType=="text")
			{
				$resultStr = $this->getTextReply($postObj);
			}
			else
			{
				$resultStr = $this->getDefaltReply($postObj);
			}
			
			$this->insertIntoDb("",$postStr,$resultStr);
			echo $resultStr;
			exit;
        }
		else
		{
        	echo "";
        	exit;
        }
    }
		
	public function checkSignature()
	{
        $signature = $_GET["signature"];
        $timestamp = $_GET["timestamp"];
        $nonce = $_GET["nonce"];
		
		$token = TOKEN;
		$tmpArr = array($token, $timestamp, $nonce);
		sort($tmpArr);
		$tmpStr = implode( $tmpArr );
		$tmpStr = sha1( $tmpStr );
		
		if( $tmpStr == $signature ){
			return true;
		}else{
			return false;
		}
	}
	
	private function getDefaltReply($postObj)
	{
		$reply = "你好，欢迎回复以下编码获取信息：\n0、所有信息\n1、弥撒及读经\n2、日祷\n".
			"3、晨祷\n4、晚祷\n5、夜祷\n6、诵读\n7、反省\n8、礼仪\n9、圣人传记\n10、代祷本";
		$textTpl = "<xml>
			<ToUserName><![CDATA[%s]]></ToUserName>
			<FromUserName><![CDATA[%s]]></FromUserName>
			<CreateTime>%s</CreateTime>
			<MsgType><![CDATA[text]]></MsgType>
			<Content><![CDATA[%s]]></Content>
			<FuncFlag>0</FuncFlag>
			</xml>";
		return sprintf($textTpl, $postObj->FromUserName, $postObj->ToUserName, time(), $reply);
	}
	
	private function getTextReply($postObj)
	{
		$modemap = array (
		'1' => 'mass',
		'2' => 'med',
		'3' => 'lod',
		'4' => 'ves',
		'5' => 'comp',
		'6' => 'let',
		'7' => 'thought',
		'8' => 'ordo',
		'9' => 'saint',
		);
		
		$fromUsername = $postObj->FromUserName;
		$toUsername = $postObj->ToUserName;
		$time = time();

		$reply = "你好，欢迎回复以下编码获取信息：\n0、所有信息\n1、弥撒及读经\n2、日祷\n".
			"3、晨祷\n4、晚祷\n5、夜祷\n6、诵读\n7、反省\n8、礼仪\n9、圣人传记\n10、代祷本";
		
		$ArtCount = 1;
		$Articles = "";
		$keyword = trim($postObj->Content);
		if($keyword=="0")
		{
			$ArtCount = 0;
			$url = "http://api.liyake.com/getstuff/getstuff.php?date=".gmdate("Y-m-d",time()+3600*8);
			$json = json_decode(file_get_contents($url),true);
			foreach ($modemap as $key => $value)
			{
				if(isset($json[$value]))
				{
					$Articles = $Articles.$this->getSubArticle($value,trim(convert_html_to_text($json[$value])),0);
					$ArtCount = $ArtCount+1;
				}
			}
		}
		else if(isset($modemap[$keyword]))
		{
			$Articles = $this->getSubArticle($modemap[$keyword],"",1);
		}
		else if($keyword=="10")
		{
		
			$textTpl = '<xml>
				<ToUserName><![CDATA[%s]]></ToUserName>
				<FromUserName><![CDATA[%s]]></FromUserName>
				<CreateTime>%s</CreateTime>
				<MsgType><![CDATA[news]]></MsgType>
				<ArticleCount>1</ArticleCount>
				<Articles>
				<item><Title><![CDATA[祈祷意向]]></Title><Url><![CDATA[http://api.liyake.com/pray/index.php]]></Url><Description><![CDATA[%s]]></Description><PicUrl><![CDATA[http://api.liyake.com/wechat/pics/comp_l1.jpg]]></PicUrl></item>
				</Articles>
				<FuncFlag>1</FuncFlag>
				</xml>';
			$resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $this->getPrays());
			return $resultStr;
		}
		
		if($Articles!="")
		{
			$textTpl = '<xml>
				<ToUserName><![CDATA[%s]]></ToUserName>
				<FromUserName><![CDATA[%s]]></FromUserName>
				<CreateTime>%s</CreateTime>
				<MsgType><![CDATA[news]]></MsgType>
				<ArticleCount>%s</ArticleCount>
				<Articles>%s
				</Articles>
				<FuncFlag>1</FuncFlag>
				</xml>';
			$resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $ArtCount, $Articles);
		}
		else
		{
			$textTpl = '<xml>
				<ToUserName><![CDATA[%s]]></ToUserName>
				<FromUserName><![CDATA[%s]]></FromUserName>
				<CreateTime>%s</CreateTime>
				<MsgType><![CDATA[text]]></MsgType>
				<Content><![CDATA[%s]]></Content>
				<FuncFlag>0</FuncFlag>
				</xml>';
			$resultStr = sprintf($textTpl, $fromUsername, $toUsername, $time, $reply);
		}
		return $resultStr;
	}
	
	private function getPrays()
	{
		$article = "";
		//先从数据库中获取
		$conn = mysql_pconnect("localhost","liyake","me_lyk");
		if($conn)
		{
			mysql_select_db("liyake",$conn);
			$result = mysql_query("select name,text,createtime from pray order by id desc limit 5;");
			while ($row = mysql_fetch_array($result))
			{
				if($article!="")
					$article = $article."\n\n";
				$article = $article.'昵称：'.$row['name'].'  时间：'.date('m-d H:i',strtotime($row['createtime'])+3600*8)."\n祈祷意向：".$row['text'];
			}
		}
		return $article;
	}
	
	private function getSubArticle($mode,$content,$isLarge)
	{
		$titlemap = array (
		'mass' => '弥撒及读经',
		'med' => '日祷经文',
		'lod' => '晨祷经文',
		'ves' => '晚祷经文',
		'comp' => '夜祷经文',
		'let' => '诵读',
		'thought' => '反省',
		'ordo' => '礼仪',
		'saint' => '圣人传记',
		);
		$title = "";
		$url = "http://api.liyake.com/getstuff/getstuff.php?date=".gmdate("Y-m-d",time()+3600*8)."&mode=".$mode;
		if($content=="")
		{
			$content = convert_html_to_text(file_get_contents($url));
		}
		$picurl = "http://api.liyake.com/wechat/pics/".$mode."1.jpg";
		if(isset($titlemap[$mode]))
			$title=$titlemap[$mode];
		$textTpl = '<item><Title><![CDATA[%s]]></Title><Url><![CDATA[%s]]></Url><Description><![CDATA[%s]]></Description><PicUrl><![CDATA[%s]]></PicUrl></item>';
		$resultStr = "";
		if($isLarge>0)
		{
			$picurl = "http://api.liyake.com/wechat/pics/".$mode."_l1.jpg";
			$index = strpos($content,"\n",140);
			$desc = "";
			if($index>0)
				$desc = substr($content,0,$index);
			else
				$desc = mb_substr($content,0,100,"UTF-8");				
				
			$resultStr = sprintf($textTpl,$title,$url,$desc, $picurl);
		}
		else
		{
			$title = $title."\n".substr($content,0,strpos($content,"\n",2));
			$resultStr = sprintf($textTpl,$title,$url,mb_substr($content,0,80,"UTF-8"), $picurl);
		}
		return $resultStr;
	}
	
	private function insertIntoDb($get,$post,$result)
	{
		$conn = mysql_pconnect("localhost","liyake","me_lyk");
		if(!$conn)
		{
			$errorcode = 3;
			die("connect db error!");
		}

		mysql_select_db("liyake",$conn);

		//检测用户名及密码是否正确
		$result = mysql_query("insert into wechat (get,post) values ('".$result."','".$_ppp."');");
	}
}
?>