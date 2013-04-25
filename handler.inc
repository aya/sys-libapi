<?php

abstract class handler
{
	private $alias = array();
	private $description = '';
	private $grants = array();
	
	public abstract function help();
	public abstract function execute();
	public abstract function fallback();
	
	public static $current = null;
	
	public function addAlias($a)
	{
		if( is_array($a) )
		{
			foreach( $a as $aa )
				$this->alias[] = $aa;
		}
		else
			$this->alias[] = $a;
	}
	
	public function setDescription($d)
	{
		$this->description = $d;
	}
	
	public function addGrant($g)
	{
		if( is_array($g) )
		{
			foreach( $g as $gg )
				$this->grants[] = $gg;
		}
		else
			$this->grants[] = $g;
	}

	public function getAlias() { return $this->alias; }
	public function getDescription() { return $this->description; }
	public function getGrants() { return $this->grants; }
	public function checkAuth() { security::requireGrants($this->grants); }
}

?>