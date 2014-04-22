<?php

class app extends ldap
{
	public $schema;
	
	public function __construct()
	{
		$this->schema = array();
		$this->schema['objectClass'] = array();
		$this->schema[] = 'uid';
		$this->schema[] = 'uidNumber';
		$this->schema[] = 'gidNumber';
		$this->schema[] = 'loginShell';
		$this->schema[] = 'mail';
		$this->schema[] = 'homeDirectory';
		$this->schema[] = 'cn';
		$this->schema[] = 'userPassword';
		$this->schema[] = 'memberUid';
		$this->schema[] = 'member';
		$this->schema[] = 'mailDeliveryOption';
		$this->schema[] = 'mailQuota';
		$this->schema[] = 'mailAlternateAddress';
		$this->schema[] = 'mailForwardingAddress';
		$this->schema[] = 'owner';
		$this->schema[] = 'description';
		$this->schema[] = 'gecos';
		$this->schema[] = 'sshPublicKey';
		$this->schema['objectClass'][] = 'top';
		$this->schema['objectClass'][] = 'applicationProcess';
		$this->schema['objectClass'][] = 'shadowAccount';
		$this->schema['objectClass'][] = 'posixAccount';
		$this->schema['objectClass'][] = 'posixGroup';
		$this->schema['objectClass'][] = 'mailRecipient';
		$this->schema['objectClass'][] = 'groupOfNames';
		$this->schema['objectClass'][] = 'inetSubscriber';
		$this->schema['objectClass'][] = 'ldapPublicKey';

		return $this->schema;
	}

	public function build($row)
	{
		$data = array();
		$data['uid'] = $row['uid'];
		if( $row['userPassword'] )
			$data['userPassword'] = $row['userPassword'];
		$data['uidNumber'] = $GLOBALS['ldap']->newUID();
		$data['gidNumber'] = $data['uidNumber'];
		$data['loginShell'] = $GLOBALS['CONFIG']['SHELL'];
		$data['mail'] = $row['uid'] . '@' . $row['domain'];
		if( $row['homeDirectory'] )
			$data['homeDirectory'] = $row['homeDirectory'];
		else
			$data['homeDirectory'] = ldap::dirFromDN($row['dn']);
		if( $row['mailForwardingAddress'] )
			$data['mailForwardingAddress'] = $row['mailForwardingAddress'];
		$data['cn'] = $row['uid'];
		$data['mailQuota'] = '100M';
		$data['mailDeliveryOption'] = 'forward';
		$data['description'] = $row['description'];
		$data['host'][] = 'proxy-001';
		$data['host'][] = 'proxy-002';
		$data['objectClass'] = $this->schema['objectClass'];
		$data['inetAuthorizedServices'][] = 'sshd';
		$data['inetAuthorizedServices'][] = 'ftp';
		$data['owner'] = ($row['owner']?"{$row['owner']}":"{$GLOBALS['CONFIG']['LDAP_ADMIN']}");
		$data['member'] = ($row['owner']?"{$row['owner']}":"{$GLOBALS['CONFIG']['LDAP_ADMIN']}");
				
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