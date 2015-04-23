<?php

require_once('Net/LDAP2.php');

class ldap
{
	private $base;
	private $config;
	private $options = array();

	const REPLACE = 1;
	const ADD = 2;
	const DELETE = 3;
	
	const DOMAIN = 1;
	const SUBDOMAIN = 2;
	const USER = 3;
	const APP = 4;
	const REPO = 5;
	const GROUP = 6;
	
	public function __construct($host, $dn, $password, $port = '389')
	{
		$this->options['host'] = $host;
		$this->options['port'] = $port;
	
		$cache_config = array(
			'path' => '/tmp/Net_LDAP2_Schema.cache',
			'max_age' =>  86400
		);

		$cache = new Net_LDAP2_SimpleFileSchemaCache($cache_config);
				
		// create a new ldap object
		$this->base = @Net_LDAP2::connect($this->options);

		if( !is_object($this->base) )
			throw new ApiException('LDAP connection failed', 500, 'Server is unavailable');
		
		// create the schema cache
		@$this->base->registerSchemaCache($cache);
		
		// ldap authentification
		$test = @$this->base->bind($dn, $password);

		if( @Net_LDAP2::isError($test) )
			throw new ApiException('LDAP bind failed', 403, 'Wrong DN or password');
	}

	public function __destruct()
	{
	
	}
	
	public function bind($dn, $password)
	{	
		// ldap authentification
		$test = @$this->base->bind($dn, $password);

		if( @Net_LDAP2::isError($test) )
			throw new ApiException('LDAP bind failed', 403, 'Wrong DN or password');
	}
	
	public function create($dn, $row)
	{
		if( @$this->base->dnExists($dn) )
			throw new ApiException('Entry already exists', 403, 'Already exists:' . $dn);

		// create the new object entry
		$object = @Net_LDAP2_Entry::createFresh($dn, $row);
		
		// push entry on ldap server
		$values = @$this->base->add($object);

		if( @Net_LDAP2::isError($values) )
			throw new ApiException('Entry creation failed', 400, 'Creation failed:' . $dn);

		return true;
	}

	public function delete($dn)
	{
		if( $dn != 'dc=dns' )
			$result = @$this->base->delete($dn, true);
			
		if( @Net_LDAP2::isError($result) )
			throw new ApiException('Entry not found', 404, 'Not found:' . $dn);

		return true;
	}

	public function replace($dn, $row, $mode = self::REPLACE)
	{
		$object = @$this->base->getEntry($dn);
		
		if( @Net_LDAP2::isError($result) )
			throw new ApiException('Entry not found', 404, 'Not found:' . $dn);

		switch( $mode )
		{
			case self::REPLACE: $values = @$this->base->modify($object, array('replace' => $row)); break;
			case self::ADD:	$values = @$this->base->modify($object, array('add' => $row)); break;
			case self::DELETE: $values = @$this->base->modify($object, array('delete' => $row)); break;
		}

		if( @Net_LDAP2::isError($values) )
			throw new ApiException('Modification failed', 400, 'Modification failed:' . $dn);
			
		return true;
	}

	public function read($dn)
	{
		$result = @$this->base->getEntry($dn);

		if( @Net_LDAP2::isError($result) )
			throw new ApiException('Entry not found', 404, 'Not found:' . $dn);

		$values = @$result->getValues();
		
		return $values;
	}

	public function search($dn, $filter, $count = false, $sort_by = 'uid', $sort = 'ASC', $scope = 'sub')
	{
		if( $count === true )
			$search = @$this->base->search($dn, $filter, array('attributes' => array('uidNumber'), 'scope' => $scope));
		else
			$search = @$this->base->search($dn, $filter, array('scope' => $scope));

		// count entries
		if( $count === true )
			return array('count'=>$search->count());
		
		if( $search->count() == 0 )
			return array();

		// sort entries
		if( $sort == 'ASC' )
			$result = @$search->sorted(array($sort_by), SORT_ASC);
		else
			$result = @$search->sorted(array($sort_by), SORT_DESC);

		// get entries values
		$i = 0;
		$values = array();
		foreach($result as $dn => $entry)
		{
			$values[$i] = @$entry->getValues();
			$values[$i]['dn'] = @$entry->dn();
			$i++;
		}

		return $values;
	}
	
	public function getUIDfromDN($dn)
	{
		$entry = $GLOBALS['ldap']->read($dn);

		if( $entry['uidNumber'] )
			return $entry['uidNumber'];
		else
			return false;	
	}
	
	public function getDNfromUID($uid)
	{
		$filter = '(uidnumber=' . $uid . ')';
		$entry = $GLOBALS['ldap']->search($GLOBALS['CONFIG']['LDAP_BASE'], $filter);
		
		if( $entry[0]['dn'] )
			return $entry[0]['dn'];
		else
			return false;	
	}

	public function getDNfromHostname($hostname)
	{
		$filter = '(associateddomain=' . $hostname . ')';
		$entry = $GLOBALS['ldap']->search($GLOBALS['CONFIG']['LDAP_BASE'], $filter);
		
		if( $entry[0]['dn'] )
			return $entry[0]['dn'];
		else
			return false;	
	}
	
	public function newUID()
	{
		$result = $GLOBALS['ldap']->read($GLOBALS['CONFIG']['LDAP_BASE']);
		$uidnumber = $result['uidNumber'];	

		$data = array('uidNumber' => $uidnumber+1);

		$GLOBALS['ldap']->replace($GLOBALS['CONFIG']['LDAP_BASE'], $data);

		return $uidnumber;
	
	}
	
	public static function buildDN($type, $domain, $value = false)
	{
		if( $value )
		{
			switch( $type )
			{
				case self::SUBDOMAIN:
					$dn = 'dc=' . str_replace('.', ',dc=', $value) . ',';	
				break;
				case self::USER:
					$dn = "uid={$value},ou=Users,"; 
				break;
				case self::GROUP:
					$dn = "cn={$value},ou=Groups,"; 
				break;	
				case self::APP:
					$dn = "cn={$value},ou=Apps,";
				break;
				case self::REPO:
					$dn = "cn={$value},ou=Repos,";
				break;
			}
		}
		
		if( $domain )
			$dn .= 'dc=' . str_replace('.', ',dc=', $domain) . ',';	

		$dn .= 'dc=dns';

		return $dn;
	}
	
	public static function buildFilter($type, $option = false)
	{
		switch( $type )
		{
			case self::DOMAIN:
				$filter = '(&' . $option . '(dc=*)(objectclass=dnsdomain)(soarecord=*))';		
			break;
			case self::SUBDOMAIN:
				$filter = '(&' . $option . '(dc=*)(objectclass=dnsdomain)(!(soarecord=*)))';
			break;
			case self::USER: 
				$filter = '(&' . $option . '(uid=*)(objectclass=person)(!(objectclass=uidObject)))';
			break;
			case self::GROUP: 
				$filter = '(&' . $option . '(uid=*)(objectclass=posixGroup)(objectclass=groupOfNames))';
			break;
			case self::APP:
				$filter = '(&' . $option . '(cn=*)(objectclass=applicationprocess)(!(objectclass=dcobject)))';
			break;
			case self::REPO:
				$filter = '(&' . $option . '(cn=*)(objectclass=applicationprocess)(objectclass=dcobject))';
			break;
		}

		return $filter;
	}
	
	public static function dirFromDN($dn)
	{
		$dn = str_replace('cn=', '', str_replace('uid=', '', str_replace('ou=', '', $dn)));
		$result = str_replace('dc=', '', str_replace(',', '/', $dn));
		$result = '/' . implode('/', array_reverse(explode('/', $result)));

		return $result;
	}	
}

?>
