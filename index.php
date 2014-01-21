<html>
<body>
<?php
   include("header.php");

   $db_handle = pg_connect('dbname=cs564_patel host=postgres.cs.wisc.edu sslmode=require');
   $query = "select max(popestimate2010), min(popestimate2010), max(popestimate2011), min(popestimate2011), max(huest_2010), min(huest_2010), max(huest_2011), min(huest_2011) from siska.pop_housing_estimate_state;";
   $result = pg_exec($db_handle, $query);
   if(!$result)
   {
      echo "error with query: ".pg_errormessage($db_handle);
      die();
   }
   if(pg_numrows($result) > 0)
   {
      $max_popest2010 = pg_result($result, 0, 0);
      $min_popest2010 = pg_result($result, 0, 1);
      $max_popest2011 = pg_result($result, 0, 2);
      $min_popest2011 = pg_result($result, 0, 3);
      $max_huest2010  = pg_result($result, 0, 4);
      $min_huest2010  = pg_result($result, 0, 5);
      $max_huest2011  = pg_result($result, 0, 6);
      $min_huest2011  = pg_result($result, 0, 7);

   }
   pg_close($db_handle)
?>
<b>SEARCH FOR NOTES</b><br /><br />
<table border=0 bgcolor='white' cellspacing=7 cellpadding=2>
<form name=input method='post' action='index.php'>
  <tr>
    <td>state name (keywords) </td>
    <td><input type='text' name='state_keywords' style='width:200px;' /></td>
  </tr>
  <tr>
    <td>region name (keywords) </td>
    <td><input type='text' name='region_keywords' style='width:200px;' /></td>
  </tr>
  <tr>
    <td>division name (keywords) </td>
    <td><input type='text' name='div_keywords' style='width:200px;' /></td>
  </tr>
  <tr>
    <td>pop estimate 2010  in range </td>
    <td><?php echo "<input type='text' name='min_popest2010' style='width:200px;' value='".$min_popest2010."' />"; ?></td>
    <td> , </td>
    <td><?php echo "<input type='text' name='max_popest2010' style='width:200px;' value='".$max_popest2010."' />"; ?></td>
  </tr>
  <tr>
    <td>pop estimate 2011 in range </td>
    <td><?php echo "<input type='text' name='min_popest2011' style='width:200px;' value='".$min_popest2011."' />"; ?></td>
    <td> , </td>
    <td><?php echo "<input type='text' name='max_popest2011' style='width:200px;' value='".$max_popest2011."' />"; ?></td>
  </tr>
  <tr>
    <td>housing estimate 2010 in range </td>
    <td><?php echo "<input type='text' name='min_huest2010' style='width:200px;' value='".$min_huest2010."' />"; ?></td>
    <td> , </td>
    <td><?php echo "<input type='text' name='max_huest2010' style='width:200px;' value='".$max_huest2010."' />"; ?></td>
  </tr>
  <tr>
    <td>housing estimate 2011 in range </td>
    <td><?php echo "<input type='text' name='min_huest2011' style='width:200px;' value='".$min_huest2011."' />"; ?></td>
    <td> , </td>
    <td><?php echo "<input type='text' name='max_huest2011' style='width:200px;' value='".$max_huest2011."' />"; ?></td>
  </tr>
  <tr>
    <td><input type='submit' value='Search' /></td>
  </tr>
</form>
</table>
<br />
<?php
    $db_handle = pg_connect('dbname=cs564_patel host=postgres.cs.wisc.edu sslmode=require');
    // Begin SELECT statement
    $selectStmt = "SELECT state, name ";
    // create an array of strings in which each element corresponds
    // to a column name within our table.
    //                  0        1         2                 3            4                 5                 6               7             8           9           10            
    $colNames = array("name","region","division","popestimate2010","popestimate2010","popestimate2011","popestimate2011","huest_2010","huest_2010","huest_2011","huest_2011"); 
    // Array elements correspond to whether or not the input field had valid
    // input in it. Elements will be false until correct input is read.
    $filledFields = array(FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
    $fieldInput = array($_POST["state_keywords"],$_POST["region_keywords"],$_POST["div_keywords"],$_POST["min_popest2010"],
           $_POST["max_popest2010"],$_POST["min_popest2011"],$_POST["max_popest2011"],$_POST["min_huest2010"],
                       $_POST["max_huest2010"],$_POST["min_huest2011"],$_POST["max_huest2011"]);
    // maps a binary value to the expected input of a field. 0 = text, 1 = integer
    $colType = array(0,0,0,1,1,1,1,1,1,1,1); 
    // holds the text which is displayed on the webpage next to the input field. used to determine where error located to print out to user
    $fieldNames = array("state name","region name", "division name", "pop estimate 2010 low value",
            "pop estimate 2010 high value", "pop estimate 2011 low value", "pop estimate 2011 high value",
                              "housing estimate 2010 low value","housing estimate 2010 high value", 
            "housing estimate 2011 low value","housing estimate 2011 high value"); 
    
    for($i = 0; $i < count($fieldInput); $i++){
      // trim the input white spaces
      $trimmedInput = trim($fieldInput[$i]);
      if(!empty($trimmedInput)){
        // determine the type of input user has entered (int or string).
        if(is_numeric($trimmedInput)){
        // error when integer and not expecting integer type
          if($colType[$i] !== 1){
            echo "Please enter text in field: " . $fieldNames[$i];
            exit;
          }
        }
        // User input a string  
        else{
          // Check if we were expecting a string
          if($colType[$i] !== 0){
            echo "Please enter an integer value in field: " . $fieldNames[$i];
            exit;
          }
        } 
        //valid type
        $selectedCols[$i] = TRUE;
      }
      // input was empty in field, ignore it and continue with loop
      else{
        $selectedCols[$i] = FALSE;
        continue;
      }
    }
   // Remove trailing comma
   $selectStmt = substr_replace($selectStmt, "", -1);
   //$selectStmt = substr_replace($selectStmt, '', -1, 0);
   // Continue constructing our select statement with what we know
   $selectStmt = $selectStmt . " FROM siska.pop_housing_estimate_state ";
   // Loop through the columns and use the ones where the $selectedCols element is true
  if(count($selectedCols) == 0){
    //want everything. no WHERE conditions
  } 
  else{
    $selectStmt = $selectStmt . "WHERE ";
    for($i = 0; $i < count($selectedCols); $i++){
      //Check if we need to include this column name in the WHERE clause
      if($selectedCols[$i] == TRUE){
        // Take care of the string input fields first
        if($i < 3){
          $selectStmt = $selectStmt . $colNames[$i] . " ILIKE " . 
          "'%" . $fieldInput[$i] . "%' AND ";
        }
        // else we're working with the numeric fields
        else{
          //low values left empty
          if($i == 4 || $i == 6 || $i == 8 || $i == 10){    
            $selectStmt = $selectStmt . $colNames[$i] . " <= " . $fieldInput[$i];
            $selectStmt = $selectStmt . " AND ";
          }
          //right fields left empty
          else{
            $selectStmt = $selectStmt . $colNames[$i] . " >= " . $fieldInput[$i];
            $selectStmt = $selectStmt . " AND ";
            // increment i right away to compare the right(max) value
            if($i !== count($selectedCols)){
              $i++;
              // max field has something in it
              if(!is_null($filledFields[$i])){
                $selectStmt = $selectStmt . $colNames[$i] . " <=" . $fieldInput[$i];
                $selectStmt = $selectStmt . " AND ";
              }
              // max field empty
              else{
                continue;
              }
            }
          }     
        }
      }
      //disregard column
      else{
        continue;
      }
    }
    //Make sure we don't have any ANDs at end of string
    if(strcmp(substr($selectStmt, -4),"AND ") == 0) {
      $selectStmt = substr($selectStmt, 0, -4);
    }
  }
  //display states in order of primary key state numbers
  $selectStmt = $selectStmt . " ORDER BY state;";
  $result = pg_query($db_handle, $selectStmt);
  if(!$result){
    echo "No results found\n";
    die();
  }
  //query success!
  else{
    //draw table
    echo "<table align='center' border ='1' cellspacing='0' cellpadding='7'>";
    //draw header
    echo "<tr><th>State</th><th>Name</th><th>Inspect</th></tr>";
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

      echo "<td>" . "<a href=inspect.php?id=" . $id . ">Inspect</a>" . "</td>";
      echo "</tr>";
    }
    echo "</table>";
  }
  pg_close($db_handle);
?>
</body>
</html>
