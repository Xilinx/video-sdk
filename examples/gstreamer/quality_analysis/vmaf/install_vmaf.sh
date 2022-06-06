#! /bin/bash

#####################################
###### Setup VVAS Environment #######
#####################################

if [[ $PATH == /opt/xilinx/vvas/bin* ]] && \
   [[ $LD_LIBRARY_PATH == /opt/xilinx/vvas/lib* ]] && \
   [[ $PKG_CONFIG_PATH == /opt/xilinx/vvas/lib/pkgconfig* ]] && \
   [[ $GST_PLUGIN_PATH == /opt/xilinx/vvas/lib/gstreamer-1.0* ]]
then
	echo "Already has VVAS environment variables set correctly"
else
	echo "Does not have VVAS environment paths. Setting using /opt/xilinx/vvas/setup.sh"
	source /opt/xilinx/vvas/setup.sh
fi


BASEDIR=$PWD

os_distr=`lsb_release -a | grep "Distributor ID:"`
os_version=`lsb_release -a | grep "Release:"`

echo $os_distr
echo $os_version

cpu_count=`cat /proc/cpuinfo | grep processor | wc -l`

echo CPU = $cpu_count



cp ./patches/* /tmp/
retval=$?
if [ $retval -ne 0 ]; then
	echo "Unable to copy patches"
        return 1
fi

# GStreamer base package installation with patch

cd /tmp/ && wget https://gstreamer.freedesktop.org/src/gst-plugins-base/gst-plugins-base-1.16.2.tar.xz && \
    tar -xvf gst-plugins-base-1.16.2.tar.xz && cd gst-plugins-base-1.16.2 && \
    patch -p1 < /tmp/0001-Add-Xilinx-s-format-support.patch
    patch -p1 < /tmp/0001-Videoaggregator-cleanup-functions.patch
retval=$?
if [ $retval -ne 0 ]; then
	echo "Unable to apply patch"
	cd $BASEDIR
	return 1
fi
    ./autogen.sh --prefix=/opt/xilinx/vvas --disable-gtk-doc && \
    make -j$cpu_count && sudo make install && \
retval=$?
if [ $retval -ne 0 ]; then
	echo "Unable to install base gstreamer plugins ($retval)"
	cd $BASEDIR
	return 1
fi
cd $BASEDIR
rm -rf /tmp/gst-plugins-base-1.16.2*


# GStreamer bad package installation
cd /tmp/ && wget https://gstreamer.freedesktop.org/src/gst-plugins-bad/gst-plugins-bad-1.16.2.tar.xz && \
    tar -xvf gst-plugins-bad-1.16.2.tar.xz && cd gst-plugins-bad-1.16.2 && \
 patch -p1 < /tmp/0001-VMAF-integration-in-gst-plugins-bad-1.16.2.patch && \
mkdir subprojects && cd subprojects && git clone https://github.com/werti/vmaf.git -b v1.3.14-gstreamer && \
cd vmaf &&  patch -p1 < /tmp/0001-Building-the-vmaf-as-dynamic-library.patch
cd /tmp/gst-plugins-bad-1.16.2 && \
meson --prefix=/opt/xilinx/vvas  --libdir=lib  build && \
cd build && ninja && sudo ninja install

retval=$?
if [ $retval -ne 0 ]; then
        echo "Unable to install bad gstreamer plugins ($retval)"
        cd $BASEDIR
        return 1
fi
cd $BASEDIR
rm -rf /tmp/gst-plugins-bad-1.16.2*


#Remove GStreamer plugin cache
rm -rf ~/.cache/gstreamer-1.0/

echo "#######################################################################"
echo "########         VMAF plugin installed successfully          ########"
echo "#######################################################################"
