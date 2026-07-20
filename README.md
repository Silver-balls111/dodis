**Dodis**:
  To-do list application made in cpp. Uses FTXUI library.

**Features**:
  - Can create a task list
  - Update the task list
  - Get contribution graph. (currently supports only one year.)

**Build/run**:
  - Clone the repo:
  ```bash
     git clone [https://github.com/Silver-balls111/dodis.git]
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
  - Make the whole ui in FTXUI.
  - Choose a suitable default color for the graph.
  - Add scroller to the graph.
  - Support image to be used in contribution graph.
