<?php

if( $GLOBALS['CONFIG']['DB_HOST'] )
{
	$GLOBALS['db'] = new mysql($GLOBALS['CONFIG']['DB_HOST'], 
								$GLOBALS['CONFIG']['DB_USERNAME'], 
								$GLOBALS['CONFIG']['DB_PASSWORD'], 
								$GLOBALS['CONFIG']['DB_DATABASE'], 
								$GLOBALS['CONFIG']['DB_PORT']);
}

if( $GLOBALS['CONFIG']['LDAP_HOST'] )
{
	$GLOBALS['ldap'] = new ldap($GLOBALS['CONFIG']['LDAP_HOST'], 
								$GLOBALS['CONFIG']['LDAP_USERNAME'], 
								$GLOBALS['CONFIG']['LDAP_PASSWORD'], 
								$GLOBALS['CONFIG']['LDAP_PORT']);
}

if( $GLOBALS['CONFIG']['SYSTEM_HOST'] )
{
	$GLOBALS['system'] = new system($GLOBALS['CONFIG']['SYSTEM_HOST']);
}
						
$GLOBALS['security'] = new security();

define('INIT_LOADED', true);

?>
