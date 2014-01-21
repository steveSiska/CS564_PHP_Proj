<html>
<body>
<?php
   include("header.php");
?>
<b>LOAD US POPULATION AND HOUSING DATA</b><br />
<table border=0 bgcolor='white' cellspacing=7 cellpadding=2>
<form name=input method='post' action='load.php'>
  <tr>
    <td>Path to CSV File</td>
    <td><input type='text' name='path' style='width:400px;' /></td>
  </tr>
  <tr>
    <td><input type='submit' value='Load' /></td>
  </tr>
</form>
</table>
<br />
<?php
  if(strcmp($_POST["path"], "") == 0){
    //do nothing
  }
  //open the file
  if(!file_exists($_POST["path"]) && !empty($_POST)){
    print "File not found";
    die();
  }
  else if(!$handle = fopen($_POST["path"], "r")){
    print "Can't open file";
    die();
  }
  //database handle
  $db_handle = pg_connect("host='postgres.cs.wisc.edu' port='5432' dbname='cs564_patel' sslmode='require'") or die("could not connect");
  //file opened successfully
  $first_flag = true; //flag to ignore first line of csv
  //loop through each line of csv and insert into table
  while(($pop_data = fgetcsv($handle,10000,",")) != FALSE){
    if($first_flag){$first_flag = false; continue;} //skip first line of csv
    //sql insert statement
    $sql = "INSERT INTO siska.pop_housing_estimate_state VALUES('$pop_data[0]', '$pop_data[1]', '$pop_data[2]', '$pop_data[3]', '$pop_data[4]', '$pop_data[5]', '$pop_data[6]', '$pop_data[7]')";
    //execute the INSERT query
    $errmsg = pg_query($db_handle, $sql);
    //print simple error message if query could not be executed
    if(!$errmsg) print "Query not executed";
  }
  //close the file
  fclose($handle);
  $count_query = "SELECT COUNT (*) FROM siska.pop_housing_estimate_state;";
  $result = pg_query($db_handle, $count_query);
  if(!$result){
    echo "Error retrieving count";
  }
  else{
    echo "Number of rows inserted: " . current(pg_fetch_row($result));
  }
  pg_close($db_handle);
?>
</body>
</html>
