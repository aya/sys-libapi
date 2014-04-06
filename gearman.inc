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

	public function sendSync($command, $host = 'sys-001')
	{
		$msg = array('command'=>$command, 'host'=>$host, 'key'=>$this->key);
		$msg = json_encode($msg);
		
		$result = $this->client->do("execCmd", $msg);

		return $result;		
	}
	
	public function sendAsync($command, $host = 'sys-001')
	{
		$msg = array('command'=>$command, 'host'=>$host, 'key'=>$this->key);
		$msg = json_encode($msg);
		
		$this->client->doBackground("execCmd", $msg);
	}
}

?>