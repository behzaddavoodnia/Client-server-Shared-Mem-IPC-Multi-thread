# A Multi-thread System-V Shared Memory Communication IPC in Client-Server manner
* This program reads a request JSON file and parses it and based on the requests in that file, tries to read some specific datasets and produces Request files in reponse of that requests.
* My program is multhithread and it usues POSIX thread library in Linux.
* For parsing the JSON requests file you can use [Jansson APIs] (https://github.com/akheron/jansson) or any other desirable parser (I recommend you if you are expert developer please use your own JSON parser :|)
* Based on the requests, we have 4 diffrent request in each request object:

**Ascending Sorting**

**Decending Sorting**

**Find Multiplicant of a given number**

**Find Dupplicates of a given number**

### Inputs:
* A requests JSON file contains some requests
* Some datasets file that contains many integer numbers.
### Return: 
* Output of clients requests and request files  
### ----------------------------------------------------------------------------

### Compilling

_Below is an example of how you can compile the source code using gcc._

1. Just use make:
   ```sh
   make run
   ```
2. After a while press:
   ```sh
   Ctrl + C
   ```