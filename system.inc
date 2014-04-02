<?php

class system
{
	const DOMAIN = 1;
	const SUBDOMAIN = 2;
	const USER = 3;
	const APP = 4;
	const GIT = 5;
	const MERCURIAL = 6;
	const SVN = 7;
	const ALIAS = 8;
	
	private $host;
	private $port;
	
	public function __construct($host = 'java', $port = '1024')
	{
		$this->host = $host;
		$this->port = $port;
	}

	public function getquota($uidnumber, $type = 'group', $max = '0')
	{
		if( $max != 0 )
			$command = "/dns/tm/sys/usr/local/bin/getuse {$uidnumber} {$type} {$max}\n";
		else
			$command = "/dns/tm/sys/usr/local/bin/getuse {$uidnumber} {$type}\n";
			
		$handler = new stream($this->host, $this->port);
		$handler->send($command);
		$result = $handler->recv(1);
		
		return $result;
	}

	public function getdatabasesize($database, $type = 'mysql', $server = 'sql.olympe.in')
	{
		$command = "/dns/tm/sys/usr/local/bin/getdbsize {$database} {$type} {$server}\n";
			
		$handler = new stream($this->host, $this->port);
		$handler->send($command);
		$result = $handler->recv(1);
		
		return $result;
	}
	
	public function getdockerstats($vm, $process, $port)
	{
		$command = "/dns/tm/sys/usr/local/bin/docker-stats {$vm} {$process} {$port}\n";
		$handler = new stream($this->host, $this->port);
		$handler->send($command);
		$result = $handler->recv(1);
		
		return $result;
	}
	
	public function exec($commands)
	{
		if( count($commands) > 0 )
		{
			foreach( $commands as $command )
			{
				$command .= " & \n";
				$handler = new stream($this->host, $this->port);
				$handler->send($command);
				sleep(2);
			}
		}
	}
}

?>
