<?php

class gearman
{
	private $client;
	private $server;
	private $key;
	
	public function __construct($server = '127.0.0.1', $key = 'admin', $port = '4730')
	{
		$this->client = new GearmanClient();
		$this->client->addServer($server, $port);
		$this->key = $key;
	}

	public function sendSync($commands, $host = 'sys-001')
	{
		if( !is_array($commands) )
			$commands = array($commands);
		
		foreach( $commands as $c )
		{
			$msg = array('command'=>$c, 'host'=>$host, 'key'=>$this->key);
			$msg = json_encode($msg);
		
			$result = $this->client->doNormal("execCmd", $msg);
		}
		
		return trim(str_replace("{$host}:", "", $result));
	}
	
	public function sendAsync($commands, $host = 'sys-001')
	{
		if( !is_array($commands) )
			$commands = array($commands);
		
		foreach( $commands as $c )
		{
			$msg = array('command'=>$c, 'host'=>$host, 'key'=>$this->key);
			$msg = json_encode($msg);
		
			$this->client->doBackground("execCmd", $msg);
		}
	}
}

?>