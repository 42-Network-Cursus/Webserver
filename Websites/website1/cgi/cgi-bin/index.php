<?php
$directory = '/Websites/cgi'; 

// Check if the specified directory exists
if (is_dir($directory)) {
    // Open the directory
    if ($handle = opendir($directory)) {
        // Create an array to store the file names
        $files = array();

        // Read all files and directories in the specified directory
        while (false !== ($file = readdir($handle))) {
            // Exclude current and parent directory entries
            if ($file != '.' && $file != '..') {
                // Add the file name to the array
                $files[] = $file;
            }
        }

        // Close the directory handle
        closedir($handle);

        // Sort the file names in ascending order
        sort($files);

        // Generate HTML code to display the file list
        echo '<ul>';
        foreach ($files as $file) {
            echo '<li>' . $file . '</li>';
        }
        echo '</ul>';
    } else {
        echo 'Unable to open the directory.';
    }
} else {
    echo 'The specified directory does not exist.';
}
?>