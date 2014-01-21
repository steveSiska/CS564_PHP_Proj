<?php session_start(); ?>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=ISO-8859-1">

<title>CS 564 PHP Project: Detail Search Result</title>
</head>
<body>

Here are the detail search results for 

<?php

//connect to the database
$dbconn = pg_connect("host=postgres.cs.wisc.edu port=5432 dbname=cs564_patel");
if ($dbconn == FALSE) {
	print "Connection to database failed.<br>";
}

//get the id of the tuple to find details for (via http GET)
$id = $_GET["id"];
$_SESSION["id"] = $id;



//construct the query
$query = "SELECT * FROM siska.pop_housing_estimate_state WHERE state = " . $id . ";";

//query the database
$result = pg_query($dbconn, $query);
if(!$result){
    echo "No results found\n";
    die();
}
//query success!
if(isset($_POST["edit"]) && (!isset($_POST["confirm"]))){
		//get state name to appear after titles
	$stquery = "SELECT name FROM siska.pop_housing_estimate_state WHERE state = " . $id. ";";

	//run state name query
	$stresult = pg_query($dbconn, $stquery);
	if(!$stresult){
		//do nothing
	}
	else{
		echo current(pg_fetch_row($stresult)) . "\n";
	}
  	//draw table
    echo "<table align='center' border ='1' cellspacing='0' cellpadding='7'>";
    //draw header
    echo "<tr><th>State</th><th>Region</th><th>Division</th><th>Name</th><th>PopEstimate2010</th><th>PopEstimate2011</th><th>Huest_2010</th><th>Huest_2011</th></tr>";
    while ($row = pg_fetch_row($result)){
      echo "<tr>";
      $count = count($row);
      $x = 0;
      $id = current($row);
      echo "<form action=\"inspect.php?id=" . $id . "\" method=\"post\">"; 
      while($x < $count){
        $c_row = current($row);
        echo "<td><input type=\"text\" name=\"".$x."\" value=\"". $c_row . "\"</td>";
        next($row);
        $x++;
      }
      echo "<input type=\"submit\" name=\"confirm\" value=\"Submit\">";
      echo "</form>";
      echo "</tr>";
    }
    echo "</table>";
  }
  else if(isset($_POST["confirm"])){
  	$update_query = "UPDATE siska.pop_housing_estimate_state SET region ='" . $_POST["1"] . "', division='" . $_POST["2"] . "', name='" . $_POST["3"] . "', popestimate2010='" . $_POST["4"] . "', popestimate2011='" . $_POST["5"] . "', huest_2010='" . $_POST["6"] . "', huest_2011='" . $_POST["7"] . "' WHERE state = " . $id . ";";
  	$update_result = pg_query($dbconn, $update_query);
  	if(!$update_result){
    	echo "Error Updating Table\n";
    	die();
	}
	else{
	//get state name to appear after titles
	$stquery = "SELECT name FROM siska.pop_housing_estimate_state WHERE state = " . $id. ";";

	//run state name query
	$stresult = pg_query($dbconn, $stquery);
	if(!$stresult){
		//do nothing
	}
	else{
		echo current(pg_fetch_row($stresult)) . "\n";
	}
	$query = "SELECT * FROM siska.pop_housing_estimate_state WHERE state = " . $id . ";";
	$update_query_result = pg_query($dbconn, $query);
	if(!$update_query_result){
    	echo "Nothing found\n";
    	die();
	}
		//draw table
    echo "<table align='center' border ='1' cellspacing='0' cellpadding='7'>";
    //draw header
    echo "<tr><th>State</th><th>Region</th><th>Division</th><th>Name</th><th>PopEstimate2010</th><th>PopEstimate2011</th><th>Huest_2010</th><th>Huest_2011</th></tr>";
    while ($row = pg_fetch_row($update_query_result)){
      echo "<tr>";
      $count = count($row);
      $x = 0;
      $id = current($row);
      while($x < $count){
        $c_row = current($row);
        echo "<td>" . $c_row . "</td>";
        next($row);
        $x++;
      }
      echo "</tr>";
    }
    echo "</table>";
	}

  }
  else{
	  	//get state name to appear after titles
	$stquery = "SELECT name FROM siska.pop_housing_estimate_state WHERE state = " . $id. ";";

	//run state name query
	$stresult = pg_query($dbconn, $stquery);
	if(!$stresult){
		//do nothing
	}
	else{
		echo current(pg_fetch_row($stresult)) . "\n";
	}
    //draw table
    echo "<table align='center' border ='1' cellspacing='0' cellpadding='7'>";
    //draw header
    echo "<tr><th>State</th><th>Region</th><th>Division</th><th>Name</th><th>PopEstimate2010</th><th>PopEstimate2011</th><th>Huest_2010</th><th>Huest_2011</th></tr>";
    while ($row = pg_fetch_row($result)){
      echo "<tr>";
      $count = count($row);
      $x = 0;
      $id = current($row);
      while($x < $count){
        $c_row = current($row);
        echo "<td>" . $c_row . "</td>";
        next($row);
        $x++;
      }
      echo "</tr>";
    }
    echo "</table>";
  }
  pg_close($dbconn);
?>
</tbody>
</table>
<?php if(!isset($_POST["edit"])) { ?>
<form action=<?php echo "inspect.php?id=" . $_GET["id"]; ?> method="post">
<input type="submit" name="edit" value="Edit">
</form>
<?php } ?>
<form action="index.php" method="post">
<input type="submit" name="back" value="Go Back to Search Page">
</form>
<!--provide a link to the previous page-->
</form>
</body>
</html>
