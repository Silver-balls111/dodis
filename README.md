**Dodis**:
  To-do list application made in cpp. Uses FTXUI library.

**Features**:
  - Can create a task list
  - Update the task list
  - Get contribution graph.

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
  - Make the cells hoverable and display the date in bottom.
  - Add option to allow editing the task file.
  - Make update() in ftxui.
  - Support image to be used in contribution graph.
  - Add some premade color pallete to match starter images.
  - Make the file path independent of the executable path
  - Create help()       [done.]
  - Support multiple year.      *done*
  - Choose a suitable default color for the graph.      [done choose grey.]
  - Fix gaps between months to be proper    [done]
  - Add horizontal scroller to the graph. [done]
