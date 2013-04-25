<?php

class ApiException extends Exception
{
	public $debug;
	
	public function __construct($message="", $code=500, $debug="", $ex=null)
	{
		if( $ex !== null )
			parent::__construct($message, $code, $ex);
		else
			parent::__construct($message, $code);
		$this->debug = $debug;
	}
	
	public function __toString()
	{
		if( !$GLOBALS['CONFIG']['DEBUG'] )
			return $this->getMessage();
		else
		{
			$s = self::stripArgs(parent::__toString());
			return $s;
		}
	}
	
	public static function stripArgs($trace)
	{
		if( is_array($trace) )
		{
			if( isset($trace['args']) )
				unset($trace['args']);
			else
				for( $i = 0; $i < count($trace); $i++ )
					unset($trace[$i]['args']);
		}
		else
			$trace = preg_replace("/^(#.+?: .*?)\\(.*?\\)$/m", "\$1()", $trace);
		
		return $trace;
	}
	
	public static function toArray($e)
	{
		$a = array('code'=>$e->getCode(), 'message'=>$e->getMessage());
		
		if( !$GLOBALS['CONFIG']['DEBUG'] )
			return $a;

		if( $e instanceof ApiException )
		{
			$a['debug'] = $e->debug;
			$a['trace'] = self::stripArgs($e->getTrace());
		}
		
		$p = $e->getPrevious();
		
		if( $p !== null )
			$a['previous'] = self::toArray($p);
		
		return $a;
	}
}

?>