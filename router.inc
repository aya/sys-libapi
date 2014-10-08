<?php

class router
{
	public static function forward($path, $alias=false)
	{
		if( $path[0] == '/' )
			request::resetPath();
		else if( $alias )
			request::popPath();
		
		$actions = preg_split("/\\s*(,|;|\\s|\\/)\\s*/", $path, -1, PREG_SPLIT_NO_EMPTY);
		for( $i = count($actions)-1; $i >= 0; $i-- )
			request::addAction($actions[$i], false);
		
		self::route();
	}
	
	public static function route()
	{
		$handler = null;
		$action_page = "";
		$path = '/'.implode('/', request::getCurrentPath());

		while( true )
		{
			if( !request::hasAction() )
			{
				if( file_exists($GLOBALS['CONFIG']['APIDIR'] . $path . '/index.php') )
				{
					define('PROPER_START', true);
					$action_page = $GLOBALS['CONFIG']['APIDIR'] . $path . '/index.php';
					$handler = require_once($action_page);
					break;
				}
				else
					throw new ApiException('', 404, 'No handler found for URI : ' . $path);
			}
			else
			{
				$action = request::getAction(false, false); // peek the action
				if( is_dir($GLOBALS['CONFIG']['APIDIR'] . $path . '/' . $action) )
				{
					request::getAction(); // pop the action
					$path .= '/'.$action;
					continue;
				}
				else if( file_exists($GLOBALS['CONFIG']['APIDIR'] . $path . '/' . $action . '.php') )
				{
					request::getAction(); // pop the action
					define('PROPER_START', true);
					$action_page = $GLOBALS['CONFIG']['APIDIR'] . $path . '/' . $action . '.php';
					$handler = require_once($action_page);
					break;
				}
				else if( file_exists($GLOBALS['CONFIG']['APIDIR'] . $path . '/index.php') )
				{
					define('PROPER_START', true);
					$action_page = $GLOBALS['CONFIG']['APIDIR'] . $path . '/index.php';
					$handler = require_once($action_page);
					break;
				}
				else
					throw new ApiException('', 404, 'No handler found for URI : ' . $path);
				
			}
		}
		
		if( $handler instanceof handler )
		{
			hook::pre($handler);
			$handler->execute();
			hook::post($handler);
		}
		else
			throw new ApiException('', 404, 'Invalid handler found for URI : ' . $path . ' using handler page : ' . $action_page);
	}
}

?>