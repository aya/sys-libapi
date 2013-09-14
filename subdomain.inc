<?php

class subdomain extends ldap
{
	public function __construct()
	{
		$this->schema = array();
		$this->schema['objectClass'] = array();
		$this->schema[] = 'dc';
		$this->schema[] = 'uid';
		$this->schema[] = 'uidNumber';
		$this->schema[] = 'gidNumber';
		$this->schema[] = 'mail';
		$this->schema[] = 'homeDirectory';
		$this->schema[] = 'cn';
		$this->schema[] = 'associatedDomain';
		$this->schema[] = 'userPassword';
		$this->schema[] = 'aRecord';
		$this->schema[] = 'cNAMERecord';
		$this->schema[] = 'loginShell';
		$this->schema[] = 'description';
		$this->schema[] = 'gecos';
		$this->schema[] = 'owner';
		$this->schema['objectClass'][] = 'top';
		$this->schema['objectClass'][] = 'domain';
		$this->schema['objectClass'][] = 'dnsDomain';
		$this->schema['objectClass'][] = 'domainRelatedObject';
		$this->schema['objectClass'][] = 'posixAccount';
		$this->schema['objectClass'][] = 'groupOfNames';	
			
		return $this->schema;
	}

	public function build($row)
	{
		$data = array();
		$data['uid'] = $row['uid'];
		$data['dc'] = $row['uid'];
		if( $row['userPassword'] )
			$data['userPassword'] = $row['userPassword'];
		$data['uidNumber'] = $GLOBALS['ldap']->newUID();
		$data['gidNumber'] = $data['uidNumber'];
		$data['homeDirectory'] = ldap::dirFromDN($row['dn']);
		$data['cn'] = $row['uid'];
		$data['associatedDomain'] = $row['subdomain'] . '.' . $row['domain'];
		$data['cNAMERecord'] = $row['domain']; 
		$data['loginShell'] = $GLOBALS['CONFIG']['SHELL'];
		$data['objectClass'] = $this->schema['objectClass'];
		
		if( $GLOBALS['CONFIG']['SYMLINK'] == 1 && $row['domain'] == $GLOBALS['CONFIG']['DOMAIN'] )
		{
			$data['objectClass'][] = 'posixGroup';
			$data['objectClass'][] = 'inetSubscriber';
			$data['objectClass'][] = 'ldapPublicKey';
		
			$data['inetSubscriberStatus'] = 'active';
			$data['inetAuthorizedServices'][] = 'sshd';
			$data['host'][] = 'on-001';
			$data['host'][] = 'on-002';
        }
		
		$data['owner'] = ($row['owner']?"{$row['owner']}":"{$GLOBALS['CONFIG']['LDAP_ADMIN']}");
		
		return $data;
	}

	public function update($row)
	{
		$data = array();
		
		foreach( $this->schema as $value )
		{
			if( !is_array($value) )
			{
				if( $row[$value] )
					$data[$value] = $row[$value];
			}
		}
		
		return $data;
	}
}

?>

