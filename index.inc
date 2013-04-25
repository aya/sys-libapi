<?php

class index extends handler
{
	private $entries = array();
	
	public function __construct()
	{
	}
	
	public function help()
	{
		$entries = $this->entries;
		for( $i = 0; $i < count($entries); $i++ )
			unset($entries[$i]['process']);

		return array('entries'=>$entries);
	}
	
	public function addEntry($path, $alias, $process=null)
	{
		if( !is_array($alias) )
			$alias = array($alias);
		$this->entries[] = array('path'=>$path, 'alias'=>$alias, 'process'=>$process);
	}
	
	public function getPath($alias)
	{
		if( !is_array($alias) )
			$alias = array($alias);
			
		foreach( $alias as $a )
			foreach( $this->entries as $e )
				if( in_array($a, $e['alias']) )
					return $e['path'];
		
		return null;
	}
	
	public function execute()
	{
		$this->checkAuth();
		
		if( request::hasAction() )
			$action = request::getAction();
		else
			throw new ApiException("Unsupported operation", 404, "Undefined action : " . $action);
		
		if( $action == 'help' || $action == 'doc' )
		{
			responder::send($this);
			return;
		}
		
		foreach( $this->entries as $e )
		{
			if( in_array($action, $e['alias']) )
			{
				if( $e['process'] !== null )
				{
					$a = $e['process'];
					$a();
				}

				router::forward($e['path'], true);
				return;
			}
		}
		
		throw new ApiException("Unsupported operation", 404, "Undefined action : " . $action);
	}
	
	public function fallback()
	{
		/* nothing to do here */
	}
}

?>