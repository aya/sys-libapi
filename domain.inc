<?php

class domain extends ldap
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
		$this->schema[] = 'mDRecord';
		$this->schema[] = 'mXRecord';
		$this->schema[] = 'nSRecord';
		$this->schema[] = 'sOARecord';
		$this->schema[] = 'manager';
		$this->schema[] = 'memberUid';
		$this->schema[] = 'mailDeliveryOption';
		$this->schema[] = 'mailQuota';
		$this->schema[] = 'mailAlternateAddress';
		$this->schema[] = 'mailForwardingAddress';
		$this->schema[] = 'mailHost';
		$this->schema[] = 'owner';
		$this->schema[] = 'gecos';
		$this->schema['objectClass'][] = 'top';
		$this->schema['objectClass'][] = 'domain';
		$this->schema['objectClass'][] = 'dnsDomain';
		$this->schema['objectClass'][] = 'domainRelatedObject';
		$this->schema['objectClass'][] = 'posixAccount';
		$this->schema['objectClass'][] = 'posixGroup';
		$this->schema['objectClass'][] = 'mailRecipient';
		$this->schema['objectClass'][] = 'groupOfNames';
		
		return $this->schema;
	}

	public function build($row)
	{
		$data = array();
		$data['dc'] = $row['uid'];
		$data['uid'] = $row['uid'];
		$data['uidNumber'] = $GLOBALS['ldap']->newUID();
		$data['gidNumber'] = $data['uidNumber'];
		$data['mail'] = 'hostmaster@' . $row['domain'];
		$data['homeDirectory'] = ldap::dirFromDN($row['dn']);
		$data['gecos'] = ($row['gecos']?"{$row['gecos']}":ldap::dirFromDN($row['dn']));
		$data['cn'] = $row['uid'];
		$data['associatedDomain'] = $row['domain'];
		$data['aRecord'] = $GLOBALS['CONFIG']['A_RECORD'];
		$data['mDRecord'] = 'virtual';
		$data['mXRecord'][] = '10 mx1.' . $GLOBALS['CONFIG']['DOMAIN'];
		$data['mXRecord'][] = '20 mx2.' . $GLOBALS['CONFIG']['DOMAIN'];
		$data['nSRecord'][] = 'ns1.' . $GLOBALS['CONFIG']['DOMAIN'];
		$data['nSRecord'][] = 'ns2.' . $GLOBALS['CONFIG']['DOMAIN'];
		$data['sOARecord'] = 'ns.' . $GLOBALS['CONFIG']['DOMAIN'] . '. hostmaster.'. $GLOBALS['CONFIG']['DOMAIN'] . '. ' . substr(time(),0,10) . ' 3600 600 604800 86400';
		$data['mailQuota'] = '100M';
		$data['mailDeliveryOption'] = 'forward';
		$data['mailAlternateAddress'] = 'postmaster@' . $row['domain'];
		$data['mailForwardingAddress'] = 'hostmaster+' . $row['domain'] . '@' . $GLOBALS['CONFIG']['DOMAIN'];
		$data['objectClass'] = $this->schema['objectClass'];
		$data['owner'] = ($row['owner']?"{$row['owner']}":"{$GLOBALS['CONFIG']['LDAP_ADMIN']}");
		
		return $data;
	}
}

?>

