<?php

// =================================
// CROSSDOMAIN
// =================================
if( preg_match("/^\\/?crossdomain.xml$/i", $_GET['__rewrite']) )
{
	header("Content-Type: text/xml");
	echo "<?xml version=\"1.0\"?><cross-domain-policy><allow-access-from domain=\"*\"/><site-control permitted-cross-domain-policies=\"all\"/><allow-http-request-headers-from domain=\"*\" headers=\"*\"/></cross-domain-policy>";
	exit;
}

// =================================
// DEFAULT PARAMETERS (OVERRIDE IN SITE CONFIG)
// =================================
$GLOBALS['CONFIG'] 					= array();
$GLOBALS['CONFIG']['CLASSPATH'] 	= __DIR__ . '/';
$GLOBALS['CONFIG']['ROOT'] 			= dirname($GLOBALS['CONFIG']['CLASSPATH']) . '/';
$GLOBALS['CONFIG']['APIDIR'] 		= $GLOBALS['CONFIG']['ROOT'] . 'api/';
$GLOBALS['CONFIG']['DEBUG'] 		= true;
$GLOBALS['CONFIG']['THROW_ERRORS'] 	= (E_ALL | E_STRICT) & ~E_NOTICE;


switch( $_SERVER["HTTP_HOST"] )
{
	case 'api.dev.anotherservice.com':
		$conf = parse_ini_file('/dns/com/anotherservice/etc/settings/api.dev.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.anotherservice.com':
		$conf = parse_ini_file('/dns/com/anotherservice/etc/settings/api.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.dev.bus-it.com':
		$conf = parse_ini_file('/dns/com/bus-it/etc/settings/api.dev.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.bus-it.com':
		$conf = parse_ini_file('/dns/com/bus-it/etc/settings/api.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.dev.olympe.in':
		$conf = parse_ini_file('/dns/in/olympe/etc/settings/api.dev.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.olympe.in':
		$conf = parse_ini_file('/dns/in/olympe/etc/settings/api.ini', true);
		$conf['type'] = 'file';
	break;
	default:
		$conf = parse_ini_file('/dns/com/anotherservice/etc/settings/api.ini', true);
		$conf['type'] = 'file';
}

if( $conf['type'] == 'env' )
{
    $services_json = getenv('VCAP_SERVICES');
    $services = json_decode($services_json, true);
    $mysql_config = $services['mysql-5.1'][0]['credentials'];
    $GLOBALS['CONFIG']['DB_HOST']           = $mysql_config['hostname'];
    $GLOBALS['CONFIG']['DB_PORT']           = $mysql_config['port'];
    $GLOBALS['CONFIG']['DB_USERNAME']       = $mysql_config['user'];
    $GLOBALS['CONFIG']['DB_PASSWORD']       = $mysql_config['password'];
    $GLOBALS['CONFIG']['DB_DATABASE']       = $mysql_config['name'];
}
else
{
    $GLOBALS['CONFIG']['DB_HOST'] 		= $conf['Main']['MYSQL_HOST'];
    $GLOBALS['CONFIG']['DB_PORT'] 		= $conf['Main']['MYSQL_PORT'];
    $GLOBALS['CONFIG']['DB_USERNAME'] 	= $conf['Main']['MYSQL_USERNAME'];
    $GLOBALS['CONFIG']['DB_PASSWORD'] 	= $conf['Main']['MYSQL_PASSWORD'];
    $GLOBALS['CONFIG']['DB_DATABASE'] 	= $conf['Main']['MYSQL_DATABASE'];
}

$GLOBALS['CONFIG']['LDAP_HOST'] 	= $conf['Main']['LDAP_HOST'];
$GLOBALS['CONFIG']['LDAP_PORT'] 	= $conf['Main']['LDAP_PORT'];
$GLOBALS['CONFIG']['LDAP_USERNAME']	= $conf['Main']['LDAP_USERNAME'];
$GLOBALS['CONFIG']['LDAP_PASSWORD']	= $conf['Main']['LDAP_PASSWORD'];
$GLOBALS['CONFIG']['LDAP_BASE']		= $conf['Main']['LDAP_BASE'];
$GLOBALS['CONFIG']['LDAP_ADMIN']	= $conf['Main']['LDAP_ADMIN'];
$GLOBALS['CONFIG']['PIWIK_URL']		= $conf['Main']['PIWIK_URL'];
$GLOBALS['CONFIG']['PIWIK_TOKEN']	= $conf['Main']['PIWIK_TOKEN'];
$GLOBALS['CONFIG']['DOMAIN']		= $conf['Main']['DOMAIN'];
$GLOBALS['CONFIG']['DEV_DOMAIN']	= $conf['Main']['DEV_DOMAIN'];
$GLOBALS['CONFIG']['JOB_SERVERS'] 	= $conf['Main']['JOB_SERVERS'];
$GLOBALS['CONFIG']['KEY'] 			= $conf['Main']['KEY'];
$GLOBALS['CONFIG']['SECRET'] 		= $conf['Main']['SECRET'];
$GLOBALS['CONFIG']['GPG_KEY'] 		= $conf['Main']['GPG_KEY'];
$GLOBALS['CONFIG']['SYMLINK'] 		= $conf['Main']['SYMLINK'];
$GLOBALS['CONFIG']['SHELL'] 		= $conf['Main']['SHELL'];
$GLOBALS['CONFIG']['GIT_TEMPLATE'] 	= $conf['Main']['GIT_TEMPLATE'];
$GLOBALS['CONFIG']['ROOT_VAR'] 		= $conf['Main']['ROOT_VAR'];
$GLOBALS['CONFIG']['ROOT_BIN'] 		= $conf['Main']['ROOT_BIN'];
$GLOBALS['CONFIG']['APP_MEMORY'] 	= $conf['Main']['APP_MEMORY'];
$GLOBALS['CONFIG']['MYSQL_ROOT_HOST']  = $conf['Main']['MYSQL_ROOT_HOST'];
$GLOBALS['CONFIG']['MYSQL_ROOT_PORT']  = $conf['Main']['MYSQL_ROOT_PORT'];
$GLOBALS['CONFIG']['MYSQL_ROOT_USER']  = $conf['Main']['MYSQL_ROOT_USER'];
$GLOBALS['CONFIG']['MYSQL_ROOT_PASSWORD']  = $conf['Main']['MYSQL_ROOT_PASSWORD'];
$GLOBALS['CONFIG']['PGSQL_ROOT_HOST']  = $conf['Main']['PGSQL_ROOT_HOST'];
$GLOBALS['CONFIG']['PGSQL_ROOT_PORT']  = $conf['Main']['PGSQL_ROOT_PORT'];
$GLOBALS['CONFIG']['PGSQL_ROOT_USER']  = $conf['Main']['PGSQL_ROOT_USER'];
$GLOBALS['CONFIG']['PGSQL_ROOT_PASSWORD']  = $conf['Main']['PGSQL_ROOT_PASSWORD'];
$GLOBALS['CONFIG']['MONGODB_ROOT_HOST']  = $conf['Main']['MONGODB_ROOT_HOST'];
$GLOBALS['CONFIG']['MONGODB_ROOT_PORT']  = $conf['Main']['MONGODB_ROOT_PORT'];
$GLOBALS['CONFIG']['MONGODB_ROOT_USER']  = $conf['Main']['MONGODB_ROOT_USER'];
$GLOBALS['CONFIG']['MONGODB_ROOT_PASSWORD']  = $conf['Main']['MONGODB_ROOT_PASSWORD'];
$GLOBALS['CONFIG']['A_RECORD']  	= $conf['Main']['A_RECORD'];
$GLOBALS['CONFIG']['SYSTEM_HOST']  	= $conf['Main']['SYSTEM_HOST'];
$GLOBALS['CONFIG']['CF_HOST']  		= $conf['Main']['CF_HOST'];
$GLOBALS['CONFIG']['CF_USERNAME']  	= $conf['Main']['CF_USERNAME'];
$GLOBALS['CONFIG']['CF_PASSWORD']  	= $conf['Main']['CF_PASSWORD'];
$GLOBALS['CONFIG']['CF_TOKEN']  	= $conf['Main']['CF_TOKEN'];

unset($conf);

$GLOBALS['CONFIG']['PARAMETERS'] 			= array();
$GLOBALS['CONFIG']['PARAMETERS']['FORMAT']	= array('f','format','output','outputformat','o');
$GLOBALS['CONFIG']['PARAMETERS']['ACTION']	= array('action');
$GLOBALS['CONFIG']['PARAMETERS']['NOHTTP']	= array('nohttperrorheader');
$GLOBALS['CONFIG']['PARAMETERS']['XML']		= array('xml','x');
$GLOBALS['CONFIG']['PARAMETERS']['JSON']	= array('json','j');
$GLOBALS['CONFIG']['PARAMETERS']['TOKEN']	= array('secret','auth','creditentials');

define('CONFIG_LOADED', true);

?>