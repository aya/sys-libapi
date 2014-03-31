<?php

class action extends handler
{
	private $returns = '';
	private $params = array();
	private $executeClosure = null;
	private $fallbackClosure = null;
	
	public function __construct()
	{
	}
	
	public function help()
	{
		return array('returns'=>$this->returns, 'params'=>$this->params);
	}
	
	public function getParams()
	{
		$data = array();
		
		foreach( $this->params as $p )
			$data[$key] = request::getCheckParam($p);

		return $data;
	}
	
	public function getParam($name)
	{
		$param = null;
		foreach( $this->params as $p )
		{
			if( in_array($name, $p['name']) )
			{
				$param = $p;
				break;
			}
		}
		
		if( $param == null )
			throw new ApiException("Internal configuration error", 500, "Action misconfiguration : parameter {$name} is not defined");
		
		return request::getCheckParam($param);
	}
	
	public function setReturn($r)
	{
		$this->returns = $r;
	}
	
	public function addParam($p)
	{
		if( !is_array($p) )
			throw new ApiException("Internal configuration error", 500, "Action misconfiguration : parameter is not compliant ".print_r($p, true));
		
		if( array_key_exists('name', $p) )
			$this->params[] = $p;
		else
		{
			foreach( $p as $pp )
				$this->params[] = $pp;
		}
	}
	
	public function setExecute($closure) { $this->executeClosure = $closure; }
	public function execute()
	{
		if( $this->executeClosure !== null )
		{
			// =================================
			// CHECK HELP
			// =================================
			$help = request::getAction(false, false);
			if( $help == 'help' || $help == 'doc' )
				responder::send($this);
		
			$a = $this->executeClosure;
			$a();
		}
	}
	
	public function setFallback($closure) { $this->fallbackClosure = $closure; }
	public function fallback()
	{
		if( $this->fallbackClosure !== null )
		{
			$a = $this->fallbackClosure;
			$a();
		}
	}
}

?>