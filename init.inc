<?php

if( $GLOBALS['CONFIG']['MYSQL_HOST'] )
{
	$GLOBALS['db'] = new mysql($GLOBALS['CONFIG']['MYSQL_HOST'], 
								$GLOBALS['CONFIG']['MYSQL_USERNAME'], 
								$GLOBALS['CONFIG']['MYSQL_PASSWORD'], 
								$GLOBALS['CONFIG']['MYSQL_DATABASE'], 
								$GLOBALS['CONFIG']['MYSQL_PORT']);
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

if( $GLOBALS['CONFIG']['GEARMAN_HOST'] )
{
	$GLOBALS['gearman'] = new gearman($GLOBALS['CONFIG']['GEARMAN_HOST'], $GLOBALS['CONFIG']['GEARMAN_KEY'], $GLOBALS['CONFIG']['GEARMAN_PORT']);
}
						
$GLOBALS['security'] = new security();

define('INIT_LOADED', true);

?>
