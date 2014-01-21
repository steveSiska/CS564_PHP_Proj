<html>
<body>
<?php
   include("header.php");
?>
<b>DROP ALL POPULATION AND HOUSING DATA</b><br />
<table border=0 bgcolor='white' cellspacing=7 cellpadding=2>
<form name=input method='post' action='delete.php'>
  <tr>
    <td>Remove all data? </td>
    <td>Yes<input type='radio' name='confirm' value='Yes' />No<input type='radio' name='confirm' value='No' checked /></td>
  </tr>
  <tr>
    <td><input type='submit' value='Proceed' /></td>
  </tr>
</form>
</table>
<br />
<?php
	//database handle
	$db_handle = pg_connect("host='postgres.cs.wisc.edu' port='5432' dbname='cs564_patel' sslmode='require'") or die("could not connect");
	//sql truncate/delate statement
    $sql = "TRUNCATE TABLE siska.pop_housing_estimate_state";
    //execute the TRUNCATE query
    $errmsg = pg_query($db_handle, $sql);
    //print simple error message if query could not be executed
    if(!$errmsg) print "Query not executed";
   // empty the POPULATION AND HOUSING table, print the number of items dropped
    pg_close($db_handle);
?>
</body>
</html>
