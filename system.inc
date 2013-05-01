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
	
	private $host;
	private $port;
	
	public function __construct($host = 'java', $port = '1024')
	{
		$this->host = $host;
		$this->port = $port;
	}
	
	public function create($type, $data)
	{
		$commands = array();
		
		switch( $type )
		{
			case self::DOMAIN:
				$commands[] = "mkdir -p {$data['homeDirectory']} && chown root {$data['homeDirectory']} && chgrp {$data['gidNumber']} {$data['homeDirectory']} && chmod 751 {$data['homeDirectory']}";
			break;
			case self::SUBDOMAIN:
				//$command = "mkdir -p {$data['homeDirectory']} && chown {$data['uidNumber']} {$data['homeDirectory']} && chgrp www-data {$data['homeDirectory']} && chmod 750 {$data['homeDirectory']}";
			break;
			case self::USER:
				$commands[] = "mkdir -p {$data['homeDirectory']} && rmdir {$data['homeDirectory']} && cp -a {$GLOBALS['CONFIG']['GIT_TEMPLATE']} {$data['homeDirectory']}.git && chown -R {$data['uidNumber']}:{$data['gidNumber']} {$data['homeDirectory']}.git && chmod 770 {$data['homeDirectory']}.git && mkdir -p {$data['homeDirectory']} && cd {$data['homeDirectory']} && git clone file://{$data['homeDirectory']}.git . && chown -R {$data['uidNumber']} {$data['homeDirectory']} && chgrp -R {$data['gidNumber']} {$data['homeDirectory']} && chmod 750 {$data['homeDirectory']}";
			break;
			case self::APP:
//				$commands[] = "cat /root/.vmc_token.template | sed -e \"s#TOKEN#{$data['token']}#g\" > /root/.vmc_token && mkdir -p {$data['homeDirectory']} && cp -a {$GLOBALS['CONFIG']['GIT_TEMPLATE']} {$data['homeDirectory']}.git && chown -R {$data['uidNumber']}:{$data['gidNumber']} {$data['homeDirectory']}.git && cd {$data['homeDirectory']} && git clone file://{$data['homeDirectory']}.git . && cp -a {$GLOBALS['CONFIG']['ROOT_VAR']}/templates/{$data['application']}/* . && cd {$data['homeDirectory']} && {$GLOBALS['CONFIG']['ROOT_BIN']}/create-manifest {$data['uid']} {$GLOBALS['CONFIG']['APP_MEMORY']} {$data['url']} {$data['framework']} {$data['runtime']} && chown -R {$data['uidNumber']} {$data['homeDirectory']} && chgrp -R {$data['gidNumber']} {$data['homeDirectory']} && chmod 770 {$data['homeDirectory']} && cd {$data['homeDirectory']} && vmc push {$data['uid']} --path ./";
				$commands[] = "cat /root/.vmc_token.template | sed -e \"s#TOKEN#{$data['token']}#g\" > /root/.vmc_token && mkdir -p {$data['homeDirectory']} && cp -a {$GLOBALS['CONFIG']['ROOT_VAR']}/templates/{$data['application']}/* {$data['homeDirectory']}/ && cd {$data['homeDirectory']} && {$GLOBALS['CONFIG']['ROOT_BIN']}/create-manifest {$data['uid']} {$GLOBALS['CONFIG']['APP_MEMORY']} {$data['url']} {$data['framework']} {$data['runtime']} && chown -R {$data['uidNumber']}:{$data['gidNumber']} {$data['homeDirectory']} && chmod 770 {$data['homeDirectory']} && cd {$data['homeDirectory']} && vmc push {$data['uid']} --path ./";
			break;
			case self::GIT:
				$commands[] = "mkdir -p {$data['homeDirectory']} && cd {$data['homeDirectory']} && cp -a {$GLOBALS['CONFIG']['GIT_TEMPLATE']}/* {$data['homeDirectory']}/ && chown -R {$data['uidNumber']}:{$data['uidNumber']} {$data['homeDirectory']} && chmod 770 {$data['homeDirectory']} && chmod -R g+w {$data['homeDirectory']} && find {$data['homeDirectory']} -type d -exec chmod g+s {} \; && cd {$data['homeDirectory']} && cd .. && ln -s {$data['uid']} {$data['description']}";
			break;
			case self::MERCURIAL:
				$commands[] = "mkdir -p {$data['homeDirectory']} && cd {$data['homeDirectory']} && hg init && chown -R {$data['uidNumber']}:{$data['uidNumber']} {$data['homeDirectory']} && chmod 770 {$data['homeDirectory']} && chmod -R g+w {$data['homeDirectory']} && find {$data['homeDirectory']} -type d -exec chmod g+s {} \; && cd {$data['homeDirectory']} && cd .. && ln -s {$data['uid']} {$data['description']}";
			break;
			case self::SVN:
				$commands[] = "mkdir -p {$data['homeDirectory']} && rmdir {$data['homeDirectory']} && svnadmin create {$data['homeDirectory']} && chown -R {$data['uidNumber']}:{$data['uidNumber']} {$data['homeDirectory']} && chmod 770 {$data['homeDirectory']} && chmod -R g+w {$data['homeDirectory']} && find {$data['homeDirectory']} -type d -exec chmod g+s {} \; && cd {$data['homeDirectory']} && cd .. && ln -s {$data['uid']} {$data['description']}";
			break;
		}
		
		$this->exec($commands);
	}

	public function delete($type, $data)
	{
		if( strlen($data['homeDirectory']) > 5 )
		{
			switch( $type )
			{		
				case self::DOMAIN:
					$commands[] = "rm -Rf {$data['homeDirectory']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}.git";
				break;
				case self::SUBDOMAIN:
					//$commands[] = "rm -Rf {$data['homeDirectory']}";
				break;
				case self::USER:
					$commands[] = "rm -Rf {$data['homeDirectory']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}.git";
				break;
				case self::APP:
					$commands[] = "rm -Rf {$data['homeDirectory']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}.git";
				break;
				case self::GIT:
					$commands[] = "cd {$data['homeDirectory']} && cd .. && rm {$data['description']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}";
				break;
				case self::MERCURIAL:
					$commands[] = "cd {$data['homeDirectory']} && cd .. && rm {$data['description']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}";
				break;
				case self::SVN:
					$commands[] = "cd {$data['homeDirectory']} && cd .. && rm {$data['description']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}";
				break;
			}
			
			$this->exec($commands);
		}		
	}
	
	public function update($type, $data, $mode)
	{
		if( strlen($data['homeDirectory']) > 5 )
		{
			switch( $type )
			{		
				case self::APP:
					if( $mode == 'add' )
						$commands[] = "ln -s {$data['homeDirectory']} {$data['targetDirectory']}";
					else if( $mode == 'delete' )
						$commands[] = "rm {$data['targetDirectory']}";
				break;
			}
			
			$this->exec($commands);
		}		
	}	

	public function getquota($uidnumber)
	{
		$command = "/dns/tm/sys/usr/local/bin/getuse {$uidnumber}\n";
		$handler = new stream($this->host, $this->port);
		$handler->send($command);
		$result = $handler->recv(1);
		
		return $result;
	}
	
	public function exec($commands)
	{
		if( is_array($commands) )
		{
			foreach( $commands as $command )
			{
				$command .= " & \n";
				$handler = new stream($this->host, $this->port);
				$handler->send($command);
			}
		}
	}
}

?>
