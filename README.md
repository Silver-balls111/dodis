**Dodis**:
  To-do list application made in cpp. Uses FTXUI library.

**Features**:
  - Can create a task list
  - Update the task list
  - Get contribution graph. (currently supports only one year.)

**Build/run**:
  - Clone the repo:
  ```bash
     git clone https://github.com/Silver-balls111/dodis.git
     cd dodis
  ```
  - Build using CMake:
  ```bash
      cmake -B build -DCMAKE_BUILD_TYPE=Release
      cmake --build build
      ./build/dodis
  ```
    
**Usage**:
  - Build using CMake as instructed above or just download the "dodis" file and run 
  ```bash
  ./dodis
  ```
  
**To-Do**:
  - Make the file path independent of the executable path
  - Fix gaps between months to be proper
  - Add option to allow editing the task file.
  - Add scroller to the graph.
  - Make the cells hoverable and display the date in bottom.
  - Support image to be used in contribution graph.
  - Create help()       [done.]
  - Support multiple year.      *done*
  - Make the whole ui in FTXUI.     [Not really i like the simple version more text based rather than button based.]
  - Choose a suitable default color for the graph.      [done choose grey.]
