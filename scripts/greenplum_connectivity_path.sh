GP_ODBC_DRIVER=unset
GP_ODBC_DRIVER_MANAGER=unset
GP_JDBC_JARFILE=unset

GPHOME_CONNECTIVITY=`pwd`
PATH=${GPHOME_CONNECTIVITY}/bin:${PATH}

if [ -d "${GPHOME_CONNECTIVITY}/drivers/odbc/${GP_ODBC_DRIVER}/${GP_ODBC_DRIVER_MANAGER}/lib/python2.5/site-packages" ]; then
    PYTHON_VER=python2.5
elif [ -d "${GPHOME_CONNECTIVITY}/drivers/odbc/${GP_ODBC_DRIVER}/${GP_ODBC_DRIVER_MANAGER}/lib/python2.6/site-packages" ]; then
    PYTHON_VER=python2.6
fi

PYTHONPATH=${GPHOME_CONNECTIVITY}/drivers/odbc/${GP_ODBC_DRIVER}/${GP_ODBC_DRIVER_MANAGER}/lib/${PYTHON_VER}/site-packages:${PYTHONPATH}
CLASSPATH=${GPHOME_CONNECTIVITY}/drivers/jdbc/${GP_JDBC_JARFILE}:${CLASSPATH}

export GPHOME_CONNECTIVITY
export PATH
export PYTHONPATH
export CLASSPATH

# Mac OSX uses a different library path variable
if [ xDarwin = x`uname -s` ]; then
  DYLD_LIBRARY_PATH=${GPHOME_CONNECTIVITY}/drivers/odbc/${GP_ODBC_DRIVER}/${GP_ODBC_DRIVER_MANAGER}/:${GPHOME_CONNECTIVITY}/lib:${DYLD_LIBRARY_PATH}
  export DYLD_LIBRARY_PATH
else
  LD_LIBRARY_PATH=${GPHOME_CONNECTIVITY}/drivers/odbc/${GP_ODBC_DRIVER}/${GP_ODBC_DRIVER_MANAGER}/:${GPHOME_CONNECTIVITY}/lib:${LD_LIBRARY_PATH}
  export LD_LIBRARY_PATH
fi

# AIX uses yet another library path variable
if [ xAIX = x`uname -s` ]; then
  LIBPATH=${GPHOME_CONNECTIVITY}/drivers/odbc/${GP_ODBC_DRIVER}/${GP_ODBC_DRIVER_MANAGER}/:${GPHOME_CONNECTIVITY}/lib:${LIBPATH}
  export LIBPATH
fi
