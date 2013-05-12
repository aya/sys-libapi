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
	
	public function create($type, $data)
	{
		$commands = array();
		
		switch( $type )
		{
			case self::DOMAIN:
				if( $GLOBALS['CONFIG']['SYMLINK'] == 1 )
					$commands[] = "mkdir -p {$data['homeDirectory']} && rmdir {$data['homeDirectory']} && mkdir -p {$data['destination']} && ln -s {$data['destination']} {$data['homeDirectory']} && chown -h {$data['site_data']['uidNumber']}:{$data['site_data']['gidNumber']} {$data['homeDirectory']} && chown {$data['site_data']['uidNumber']}:{$data['site_data']['gidNumber']} {$data['destination']} && cd {$data['homeDirectory']} && mkdir Users && chown {$data['site_data']['uidNumber']}:{$data['site_data']['gidNumber']} Users && chmod g+s Users && ln -s . www && chown -h {$data['site_data']['uidNumber']}:{$data['site_data']['gidNumber']} www";
				else
					$commands[] = "mkdir -p {$data['homeDirectory']} && chown root:{$data['gidNumber']} {$data['homeDirectory']} && chmod 755 {$data['homeDirectory']} && cd {$data['homeDirectory']} && echo \"RewriteEngine On\" > .htaccess  && echo \"RewriteCond %{HTTP_HOST} ^".str_replace(".", "\\.", $data['associatedDomain'])."\$\" [NC]  >> .htaccess && echo \"RewriteRule ^(.*)$ http://www.{$data['associatedDomain']}/\\\$1 [QSA,L,R=301]\" >> .htaccess";
			break;
			case self::ALIAS:
				if( $data['type'] == 'permanent' )
					$commands[] = "mkdir -p {$data['homeDirectory']} && cd {$data['homeDirectory']} && echo \"RewriteEngine On\" > .htaccess && echo \"RewriteCond %{HTTP_HOST} ^".str_replace(".", "\\.", $data['associatedDomain'])."\$\" [NC]  >> .htaccess && echo \"RewriteRule ^(.*)$ http://www.{$data['associatedDomain']}/\\\$1 [R=302,L]\" >> .htaccess && echo \"RewriteCond %{HTTP_HOST} ^(.+)\\.".str_replace(".", "\\.", $data['associatedDomain'])."\$\" >> .htaccess && echo \"RewriteRule ^(.*)$ http://%1.{$data['source']['associatedDomain']} [QSA,L,R=301]\" >> .htaccess";
				else
					$commands[] = "ln -s {$data['source']['homeDirectory']} {$data['homeDirectory']}";
			break;
			case self::SUBDOMAIN:
				if( $GLOBALS['CONFIG']['SYMLINK'] == 1 )
					$commands[] = "mkdir -p {$data['homeDirectory']} && chown {$data['uidNumber']}:33 {$data['homeDirectory']} && chmod 750 {$data['homeDirectory']}";
			break;
			case self::USER:
				if( $GLOBALS['CONFIG']['SYMLINK'] == 1 )
					$commands[] = "mkdir -p {$data['homeDirectory']} && chown {$data['uidNumber']}:33 {$data['homeDirectory']} && chmod 770 {$data['homeDirectory']} && chmod g+s {$data['homeDirectory']}";
				else
					$commands[] = "mkdir -p {$data['homeDirectory']} && chown {$data['uidNumber']}:{$data['gidNumber']} {$data['homeDirectory']} && chmod 770 {$data['homeDirectory']}";
			break;
			case self::APP:
				$commands[] = "{$GLOBALS['CONFIG']['ROOT_BIN']}/create-app {$data['uid']} {$data['homeDirectory']} {$data['token']} {$data['uidNumber']} {$data['gidNumber']} {$data['application']} {$GLOBALS['CONFIG']['APP_MEMORY']} {$data['url']} {$data['framework']} {$data['runtime']}";
				$commands[] = "sleep 5 && cd {$data['homeDirectory']} && vmc push {$data['uid']} --path ./";
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
		$commands = array();
		
		if( strlen($data['homeDirectory']) > 5 )
		{
			switch( $type )
			{		
				case self::DOMAIN:
					$commands[] = "rm -Rf {$data['homeDirectory']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}.git";
				break;
				case self::SUBDOMAIN:
					$commands[] = "rm -Rf {$data['homeDirectory']}";
				break;
				case self::USER:
					$commands[] = "rm -Rf {$data['homeDirectory']}";
					$commands[] = "rm -Rf {$data['homeDirectory']}.git";
				break;
				case self::APP:
					if( $data['description'] )
					{
						$envs = json_decode($data['description'], true);
						foreach( $envs as $e )
							$commands[] = "cd {$data['homeDirectory']} && cd ../../ && cd {$e} && rm -Rf Apps/{$data['uid']}";
					}
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
	
	public function update($type, $data, $mode = 'add')
	{
		$commands = array();
		
		if( strlen($data['homeDirectory']) > 5 )
		{
			switch( $type )
			{		
				case self::USER:
					$commands[] = "cat /root/.vmc_token.template | sed -e \"s#TOKEN#{$data['token']}#g\" > {$data['homeDirectory']}/.vmc_token && cp /root/.vmc_target {$data['homeDirectory']}/.vmc_target && chown {$data['uidNumber']}:{$data['gidNumber']} {$data['homeDirectory']}/.vmc* && chmod 660 {$data['homeDirectory']}/.vmc*";
				break;
				case self::APP:
					if( $mode == 'add' )
					{
						$commands[] = "ln -s {$data['homeDirectory']} {$data['data2']['homeDirectory']}";
						if( count($data['homes']) > 0 )
						{
							foreach( $data['homes'] as $h )
								$commands[] = "cd {$h} && ln -s Apps/{$data['uid']} {$data['data2']['uid']}";
						}		
					}
					else if( $mode == 'delete' )
					{
						$commands[] = "rm {$data['data2']['homeDirectory']}";
						if( count($data['homes']) > 0 )
						{
							foreach( $data['homes'] as $h )
								$commands[] = "cd {$h} && rm {$data['data2']['uid']}";
						}
					}
					else if( $mode == 'add-env' )
						$commands[] = "{$GLOBALS['CONFIG']['ROOT_BIN']}/create-env {$data['env']} {$data['uid']} {$data['homeDirectory']} {$data['domain']['homeDirectory']} {$data['uidNumber']} {$data['gidNumber']}";
					else if( $mode == 'del-env' )
						$commands[] = "{$GLOBALS['CONFIG']['ROOT_BIN']}/delete-env {$data['env']} {$data['uid']} {$data['homeDirectory']} {$data['domain']['homeDirectory']} {$data['uidNumber']} {$data['gidNumber']}";
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
