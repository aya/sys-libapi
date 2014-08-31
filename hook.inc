<?php

class hook
{
	private static function script($leading)
	{
		if( !isset($GLOBALS['CONFIG']['HOOK_DIR']) )
			return null;
		
		$file = $GLOBALS['CONFIG']['HOOK_DIR'] . '/' . ($leading ? $leading . '-' : '') . implode(request::getCurrentPath(), '-');
		
		if( file_exists($file) && is_executable($file) )
			return $file;
		else
			return null;
	}
	
	public static function run($handler, $mode)
	{
		if( !($handler instanceof action) )
			return;
		
		$file = self::script($mode);
		if( $file == null )
			return;
			
		$p = $handler->getParams();
		$cmd = $file;
		foreach( $p as $key => $value )
			$cmd .= ' ' . escapeshellarg($key . '=' . $value);
		
		$GLOBALS['system']->exec(array($cmd));
	}
	
	public static function pre($handler)
	{
		self::run($handler, 'pre');
	}
	
	public static function post($handler)
	{
		self::run($handler, 'post');
	}
}
?>