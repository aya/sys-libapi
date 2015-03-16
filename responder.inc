<?php

class responder
{
	const JSON = 1;
	const XML = 2;
	const PHP = 4;
	const DUMP = 8;
	const HTML = 16;
	
	private static $currentFormat = 0;
	public static $defaultFormat = 1; // default to JSON
	
	private static function afterSend($andexit)
	{
		if( $andexit )
			exit;
	}
	
	private static function beforeSend($result)
	{
		$log = array("user" => security::getUser('user_id'),
					 "username" => security::getUser('user_name'),
					 "date" => time(),
					 "path" => request::getCurrentPath(),
					 "ip" => request::getRemoteIp(),
					 "error" => ($result instanceof Exception ? $result->getMessage() : null),
					 "attachments" => array(),
					 "time" => round((microtime(true) - __EPOCH__) * 1000)
		);
		
		request.clearParam($GLOBALS['CONFIG']['PARAMETERS']['FORMAT']);
		request.clearParam($GLOBALS['CONFIG']['PARAMETERS']['TOKEN']);
		request.clearParam($GLOBALS['CONFIG']['PARAMETERS']['NOHTTP']);
		request.clearParam(array('pass', 'password', 'user_password', 'user_pass'));
		
		$params = request.getParams();
		if( preg_match($log['path'], "/^\\/?(self\\/)?tokens?\\/(select|list|view|search).*/is") )
			params = array();
		$log["parameters"] = $params;

		logger::benchmark($log);
		return $result;
	}
	
	// DEPRECATED
	public static function sendError($e)
	{
		self::send($e);
	}
	
	// DEPRECATED
	public static function help($h)
	{
		self::send($h);
	}
	
	public static function send($result=null, $format=0, $andexit=true)
	{
		$result = self::beforeSend($result);
		
		if( $result === null )
		{
			self::afterSend($andexit);
			return;
		}
		
		if( $format > 0 )
			self::$currentFormat == $format;

		$is_error = false;
		if( $result instanceof Exception )
		{
			$is_error = true;
			$result = self::getError($result);
		}
		if( $result instanceof handler )
		{
			$result = self::getHelp($result);
		}
		
		switch(self::$currentFormat)
		{
			case self::JSON: 
				if( $is_error )
					echo json_encode(array('error'=>$result)); 
				else
					echo json_encode(array('response'=>$result)); 
				break;
			case self::XML: echo xml::encode($result); break;
			case self::PHP: echo serialize($result); break;
			case self::DUMP: echo print_r($result, true); break;
			case self::HTML: 
				$result = self::htmlize($result);
				echo self::template($result); break;
		}
		
		self::afterSend($andexit);
	}
	
	public static function detectFormat()
	{
		$format = request::getParam($GLOBALS['CONFIG']['PARAMETERS']['FORMAT']);
		if( $format === null )
		{
			self::$currentFormat = self::$defaultFormat;
			return;
		}

		switch(strtolower($format))
		{
			case 'json': header("Content-Type: application/json"); self::$currentFormat = self::JSON; return;
			case 'xml': header("Content-Type: text/xml"); self::$currentFormat = self::XML; return;
			case 'php': header("Content-Type: text/plain"); self::$currentFormat = self::PHP; return;
			case 'dump': header("Content-Type: text/plain"); self::$currentFormat = self::DUMP; return;
			case 'html': header("Content-Type: text/html"); self::$currentFormat = self::HTML; return;
			default:
				throw new ApiException('', 400, 'Invalid output format : ' . $format);
		}
	}
	
	private static function getError($e)
	{
		$nohttp = true;//request::getParam($GLOBALS['CONFIG']['PARAMETERS']['NOHTTP']);

		// check if the error should send a HTTP header
		if( $nohttp === null || !$nohttp )
		{
			switch($e->getCode())
			{
				case 200: header("HTTP/1.1 200 OK"); break;
				case 400: header("HTTP/1.1 400 Bad Request"); break;
				case 401: header("HTTP/1.1 401 Unauthorized"); break;
				case 403: header("HTTP/1.1 403 Forbidden"); break;
				case 404: header("HTTP/1.1 404 Not Found"); break;
				case 405: header("HTTP/1.1 405 Method Not Allowed"); break;
				case 406: header("HTTP/1.1 406 Not Acceptable"); break;
				case 409: header("HTTP/1.1 409 Conflict"); break;
				case 412: header("HTTP/1.1 412 Precondition Failed"); break;
				default:
				case 500: header("HTTP/1.1 500 Internal Server Error"); break;
				case 501: header("HTTP/1.1 501 Not Implemented"); break;
				case 503: header("HTTP/1.1 503 Service Unavailable"); break;
			}
		}
		
		return ApiException::toArray($e);
	}

	private static function htmlize($object)
	{
		$html = '';
		
		if( is_array($object) )
		{
			$html .= '<ul>';
			foreach( $object as $key=>$value )
			{
				$html .= '<li><strong>'.$key.'</strong>: '.self::htmlize($value).'</li>';
			}
			$html .= '</ul>';
		}
		else
			$html .= $object;
		
		return $html;
	}
	
	private static function template($body)
	{
		$b = "<!DOCTYPE html>
<html>
<head>
<title>API Help</title>
<style>
	li { list-style-type: circle; }
	ul li > ul { border : 1px dashed #CCCCCC; padding: 10px; margin: 0px; margin-left: 25px; }
	ul li > ul li { margin-left: 20px; }
	ul li > ul li > ul { border: 0px none white; }
	h3 { margin: 0px; display: inline; font-size: 1em; font-weight: normal; font-style: italic; color: #668899; }
	h2 { margin: 0px; display: inline; font-size: 16px; color: #668899; }
	h1 { margin: 10px; font-size: 20px; border-top: 2px solid #99BBCC; border-bottom: 2px solid #99BBCC; background-color: #EEF5FF; padding: 8px; color: #668899; }
	h1 a, h2 a, a { text-decoration: underline; color: #668899; }
	h3.label { width: 120px; display: inline-block; }
	span.check { color: red; margin-left: 30px; }
	span.required { font-weight: bold; color: #FF7575; }
	span.urlizable { font-weight: bold; color: #7979FF; }
	span.multiple { font-weight: bold; color: #27DE55; }
	span.optional { font-weight: bold; color: #DFE32D; }
</style>
</head>
<body>
<h1><a href=\"/help?f=html\">API Help</a>";
		
		$p = request::getCurrentPath();
		$path = '/';
		for( $i = 0; $i < count($p); $i++ )
		{
			if( $i < count($p)-1 )
			{
				$path .= $p[$i].'/';
				$b .= " :: <a href=\"{$path}help?f=html\">{$p[$i]}</a>";
			}
			else
				$b .= " :: {$p[$i]}";
		}
		
		$b .= "</h1>";
		
		$b .= $body;
		$b .= "
</body>
</html>";
		return $b;
	}
	
	private static function getHelp($handler)
	{
		$data = $handler->help();
		$data['alias'] = $handler->getAlias();
		$data['description'] = $handler->getDescription();
		$data['grants'] = $handler->getGrants();
		
		if( self::$currentFormat != self::HTML )
		{
			return $data;
		}
		
		$body = "
<ul>
	<li><h2>Alias :</h2> ".implode(', ', $data['alias'])."</li>
	<li><h2>Description :</h2> {$data['description']}</li>
	<li><h2>Required grants :</h2> ".implode(', ', $data['grants'])."</li>";

		if( $handler instanceof index )
		{
			$body .= "
	<li><h2>Entries :</h2>
		<ul>";
			foreach( $data['entries'] as $e )
				$body .= "\n			<li><h2><a href=\"{$e['path']}/help?f=html\">{$e['alias'][0]}</a></h2> (alias : ".implode(', ', $e['alias']).")</li>";
			$body .= "
		</ul>
	</li>";
		}
		else if( $handler instanceof action )
		{
			$body .= "
	<li><h2>Parameters :</h2>
		<ul>";
			
			foreach( $data['params'] as $p )
			{
				$body .= "<li>{$p['name'][0]} : {$p['description']} ";
				if( $p['optional'] == false )
					$body .= "<span class=\"required\">required</span>. ";
				else
					$body .= "<span class=\"optional\">optional</span> ";
				if( $p['array'] == true )
					$body .= "<span class=\"multiple\">multiple</span> ";
				if( $p['action'] == true )
					$body .= "<span class=\"urlizable\">urlizable</span> ";
				$body .= "<ul><li><h3>alias :</h3> ".implode(', ', $p['name'])."</li>";
				
				if( isset($p['minlength']) )
					$body .= "<li><h3>min length : </h3> {$p['minlength']}</li>";
				if( isset($p['maxlength']) )
					$body .= "<li><h3>max length : </h3> {$p['maxlength']}</li>";
				if( isset($p['match']) )
					$body .= "<li><h3>must match : </h3> ".request::getRegex($p['match'])."</li>";
				
				$body .= "</ul></li>";
			}
		
			$body .= "
		</ul>
	</li>
	<li><h2>Returns :</h2> ".json_encode($data['returns'])."</li>
	<li><h2>Launch this action :</h2>
		<script type=\"text/javascript\">
			var rules = {
				'auth': /^[a-zA-Z0-9\\.\\\\-]{3,25}:[a-zA-Z0-9]{32,32}$/";
			
			foreach( $data['params'] as $p )
			{
				$body .= ",
				'{$p['name'][0]}': ";
				
				if( isset($p['match']) )
					$body .= rtrim(request::getRegex($p['match']), 's');
				else
					$body .= "/[\s\S]*/";
			}
			
			$body .= "
			};
			
			function checkInput(name)
			{
				if( rules[name] )
				{
					if( rules[name].test(document.getElementById('input_'+name).value) )
						document.getElementById('check_'+name).innerHTML = 'Simple check OK';
					else
						document.getElementById('check_'+name).innerHTML = 'Check did not pass';
				}
			}
		</script>
		<form action=\"/".implode('/', request::getCurrentPath())."?f=html\" method=\"post\" style=\"margin-left: 50px;\">";
	
			foreach( $data['params'] as $p )
			{
				$body .= "
			<h3 class=\"label\">{$p['name'][0]}</h3> <input type=\"text\" id=\"input_{$p['name'][0]}\" name=\"{$p['name'][0]}\" onkeyup=\"checkInput('{$p['name'][0]}');\" /><span class=\"check\" id=\"check_{$p['name'][0]}\"></span><br />";
			}
			
			$body .= "
			<h3 class=\"label\">creditentials</h3> <input type=\"text\" id=\"input_auth\" name=\"auth\" onkeyup=\"checkInput('auth');\"  /><span class=\"check\" id=\"check_auth\"></span><br />
			<input type=\"submit\" value=\"Submit\" />
		</form>
	</li>";
		}
		
		$body .= "\n</ul>";
		return $body;
	}
}

?>