# Problem 2 - MATRIX MULTIPLICATION
- Preprocessing the local input file to output key value pairs of matrix 
  elements where key is the (row, col, expression) address this element will take in the resultant matrix and value is the actual value of the element in the matrix, and storing this into a local temporary input file.
- Copied this temp file containing key value pairs to docker container, alongside the mapper.py and reducer.py.
- Copied the recently pasted input file inside the docker container to hdfs dfs as input to mapper.
- removing the local temp input file (optional)
- Mapper arranges the key value pairs input to it by using as key the 
  elements that are to be multiplied together and that are to be added 
  together during multiplication and passes the result to reducer
- Reducer multiplies adjacent elements and adds the resultant 
  elements to form one element of multiplication.
- showing output in hdfs dfs /output on terminal
# Problem 3 - EULER’S CONSTANT
- Copied this temp file containing key value pairs to docker container, alongside the mapper.py and reducer.py.
- Copied the recently pasted input file inside the docker container to hdfs dfs as input to mapper.
- running mapper, with n_iter iterations, and each time computing the n with adding values between (0, 1), and giving such as values along with some keys to reducer.
- reducer taking these and finding the average value to find the expected value of <b>e</b>
- also displaying graphs of accuracy vs iterations and confidence interval vs iterations