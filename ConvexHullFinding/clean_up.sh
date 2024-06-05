if [ -e "cmake_install.cmake" ]; then
    rm cmake_install.cmake
fi    

if [ -e "CMakeCache.txt" ]; then
    rm CMakeCache.txt
fi       

if [ -e "Makefile" ]; then
    rm Makefile
fi   

if [ -e "CMakeFiles" ]; then
    rm -r CMakeFiles    
fi   

if [ -e "App/CMakeFiles" ]; then
    rm -r App/CMakeFiles  
fi   

if [ -e "App/Makefile" ]; then
    rm App/Makefile 
fi   

if [ -e "App/cmake_install.cmake" ]; then
    rm App/cmake_install.cmake    
fi       