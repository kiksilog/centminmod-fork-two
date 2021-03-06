selinxcheck() {
	# check if selinux is disabled, if not abort and prompt to reboot

	if [ ! -f /usr/sbin/sestatus ]; then
		yum -q -y install policycoreutils
	fi

	if [ ! -f /usr/sbin/setenforce ]; then
		yum -q -y install libselinux-utils
	fi	

	if [[ -z $(sestatus | grep disabled) && -f /etc/selinux/config ]]; then
		echo "---------------------------------------------------------------"
		echo "SELINUX linux detected"
		echo "---------------------------------------------------------------"
		echo "disabling SELINUX..."
		echo "will require a server reboot before running initial install"
		sleep 3
		sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
		sed -i 's/SELINUX=permissive/SELINUX=disabled/g' /etc/selinux/config && setenforce 0
		echo
		echo "SELINUX disabled, please reboot server and rerun install"
		echo "on reboot you can find the centmin.sh file located at:"
		echo "echo $(pwd)"
		echo "---------------------------------------------------------------"
		exit 0
	fi
}

blockeditorcheck() {
	if [[ -z "$(grep 17234 /usr/local/nginx/conf/drop.conf)" ]]; then
		if [ -f "${SCRIPT_DIR}/config/nginx/drop.conf" ]; then
		rm -rf /usr/local/nginx/conf/drop.conf
		\cp -f ${SCRIPT_DIR}/config/nginx/drop.conf /usr/local/nginx/conf/drop.conf
		fi
	fi
}

centaltoff() {
	if [[ -f /etc/yum.repos.d/centalt.repo ]]; then
		sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/centalt.repo
		yum clean all -q
	fi
}

csfipsetcheck() {
	if [[ ! -f /proc/user_beancounters && "$(lsmod | grep ip_set)" ]]; then
		if [[ ! -f /usr/sbin/ipset || ! -f /usr/include/libipset/data.h ]]; then
			yum -y -q install ipset ipset-devel
			if [ -f /etc/csf/csf.conf ]; then
			sed -i 's/LF_IPSET = "0"/LF_IPSET = "1"/g' /etc/csf/csf.conf
			fi
		fi
	else
		if [ -f /etc/csf/csf.conf ]; then
		sed -i 's/LF_IPSET = \"1\"/LF_IPSET = \"0\"/' /etc/csf/csf.conf
		fi
	fi
}

checkaliases() {
	if [[ "$(grep 'centminmod-123.08centos7beta01' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta01/centminmod/' /root/.bashrc
    fi
    if [[ "$(grep 'centminmod-123.08centos7beta02' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta02/centminmod/' /root/.bashrc
    fi
    if [[ "$(grep 'centminmod-123.08centos7beta03' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08centos7beta03/centminmod/' /root/.bashrc
	fi
	if [[ "$(grep 'centminmod-123.08beta03' /root/.bashrc)" ]]; then
        sed -i 's/centminmod-123.08beta03/centminmod/' /root/.bashrc        
    fi
}

checkcmdircmd() {
	if [[ "$(grep 'cmdir=' /root/.bashrc)" || "$(grep 'centmin=' /root/.bashrc)" ]]; then
		sed -i '/cmdir=/d' /root/.bashrc
		sed -i '/centmin=/d' /root/.bashrc
		rm -rf /usr/bin/cmdir
		alias cmdir="pushd ${SCRIPT_DIR}"
		echo "alias cmdir='pushd ${SCRIPT_DIR}'" >> /root/.bashrc
cat > "/usr/bin/centmin" << EOF
#!/bin/bash
pushd $SCRIPT_DIR; bash centmin.sh
EOF
		chmod 0700 /usr/bin/centmin
	fi
}

nvcheck() {
	if [[ -f "${SCRIPT_DIR}/tools/nv.sh" && -f /usr/bin/nv ]]; then
		# auto update /usr/bin/nv if different from centmin mod tools/nv.sh copy
		# if [[ "$(diff -qr ${SCRIPT_DIR}/tools/nv.sh /usr/bin/nv; echo $?)" != '0' ]]; then
		\cp -f ${SCRIPT_DIR}/tools/nv.sh /usr/bin/nv
		chmod +x /usr/bin/nv
		# fi
	else
		\cp -f ${SCRIPT_DIR}/tools/nv.sh /usr/bin/nv
		chmod +x /usr/bin/nv
	fi		
}

multiphpcheck() {
	if [[ -d /usr/local/nginx/conf ]] && [[ ! -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf || ! -f /usr/local/nginx/conf/php-pool5.conf ]]; then
		# check to see if multiple php-fpm pool files exist and copy over if they don't
		# exist on existing server
		if [[ -f "${SCRIPT_DIR}/config/nginx/php-pool5.conf" ]]; then
			\cp -f ${SCRIPT_DIR}/config/nginx/php-pool* /usr/local/nginx/conf/
		fi
		if [[ -f "${SCRIPT_DIR}/config/nginx/phpfpmd/phpfpm_pool2.conf" && ! -f /usr/local/nginx/conf/phpfpmd/phpsocket1.conf ]]; then
			\cp -f ${SCRIPT_DIR}/config/nginx/phpfpmd/phpfpm_pool* /usr/local/nginx/conf/phpfpmd/
			if [[ "$USEEDITOR" = 'vim' ]]; then
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
    			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5
			else
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
    			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5    
			fi # USEEDITOR			
		fi
	fi

	if [[ -f /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf && ! -f /usr/bin/fpmconf-2 ]]; then
		if [[ "$USEEDITOR" = 'vim' ]]; then
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
   			echo "vim /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5
		else
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool2.conf" >/usr/bin/fpmconf-2 ; chmod 700 /usr/bin/fpmconf-2
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool3.conf" >/usr/bin/fpmconf-3 ; chmod 700 /usr/bin/fpmconf-3
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool4.conf" >/usr/bin/fpmconf-4 ; chmod 700 /usr/bin/fpmconf-4
   			echo "nano -w /usr/local/nginx/conf/phpfpmd/phpfpm_pool5.conf" >/usr/bin/fpmconf-5 ; chmod 700 /usr/bin/fpmconf-5    
		fi # USEEDITOR	
	fi	
}

mjemalloc() {
    if [[ "$MARIADB_JEMALLOC" = [yY] && ! "$(grep libjemalloc /usr/bin/mysqld_safe)" && -f /usr/lib64/libjemalloc.so.1 ]]; then
        # echo "switching MariaDB to jemalloc malloc method"
        sed -i 's|^mysqld_ld_preload=|mysqld_ld_preload=/usr/lib64/libjemalloc.so.1|' /usr/bin/mysqld_safe
        # service mysql restart
        # pmap `pidof mysqld` | grep jemalloc
    fi    
}

fixlogrotate() {
	if [ -z "$(grep '\$' /etc/logrotate.d/nginx)" ]; then
		# sed -i "s|kill.*|kill -SIGUSR1 \$(cat \/usr\/local\/nginx\/logs\/nginx.pid 2>\/dev\/null) 2>\/dev\/null \|\| true|g" /etc/logrotate.d/nginx
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        size=500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
	fi

	if [ -z "$(grep '\$' /etc/logrotate.d/php-fpm)" ]; then
		# sed -i "s|kill.*|kill -SIGUSR1 \$(cat \/var\/run\/php-fpm\/php-fpm.pid 2>\/dev\/null) 2>\/dev\/null \|\| true|g" /etc/logrotate.d/php-fpm
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        size=500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
	fi
}

leupdates() {
	# update .well-known content-type
	CHECKCONTENTTYPE=$(grep 'application\/jose+json' /usr/local/nginx/conf/staticfiles.conf)
	if [ "$CHECKCONTENTTYPE" ]; then
		sed -i "s|application\/jose+json|text\/plain|" /usr/local/nginx/conf/staticfiles.conf
	fi
}

memcachedupdatechecks() {
	# 1.4.25 added options https://community.centminmod.com/threads/memcached-1-4-25-released.5007/
	if [[ "$INITIALINSTALL" != [yY] ]]; then
		if [[ -z "$(grep 'MEM_CURVER' /etc/init.d/memcached)" ]]; then
			if [[ -f /usr/local/src/centminmod/config/memcached/memcached1425.patch && -f /etc/init.d/memcached ]]; then
				\cp -f /usr/local/src/centminmod/config/memcached/memcached1425.patch /etc/init.d/memcached1425.patch
				cd /etc/init.d/
				patch -s < memcached1425.patch
				rm -rf memcached1425.patch
			fi
		fi
	fi
}

cpcheck() {

if [ -f /var/cpanel/cpanel.config ]; then
echo "WHM/Cpanel detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/psa/.psa.shadow ]; then
echo "Plesk detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [ -f /etc/init.d/directadmin ]; then
echo "DirectAdmin detected.. centmin mod NOT compatible"
echo "aborting script..."
exit
fi

if [[ "$INITIALINSTALL" != [yY] ]]; then
	selinxcheck
	centaltoff
	csfipsetcheck
	checkaliases
	checkcmdircmd
	nvcheck
	multiphpcheck
	mjemalloc
	blockeditorcheck
	rpcnfsports
	fixlogrotate
	leupdates
	memcachedupdatechecks
fi

}

histformat() {
	if [[ -z "$(grep HISTTIMEFORMAT /root/.bashrc)" ]]; then
		echo "HISTTIMEFORMAT=\"[%d.%m.%y] %T   \"" >> /root/.bashrc
		echo "export HISTSIZE=10000" >> /root/.bashrc
		echo "export HISTTIMEFORMAT" >> /root/.bashrc
	fi	
}

setupdate() {
cat > "/usr/bin/cminfo_updater"<<EOF
#!/bin/bash
rm -rf /usr/bin/cminfo
wget -q --no-check-certificate -O /usr/bin/cminfo https://raw.githubusercontent.com/centminmod/centminmod/master/tools/cminfo.sh
chmod 0700 /usr/bin/cminfo
EOF

chmod 0700 /usr/bin/cminfo_updater

# insert itself into cronjob for auto updates
if [ ! -f /usr/bin/crontab ]; then
	yum -y -q install cronie
		if [[ "$CENTOS_SEVEN" != '7' ]]; then
			service crond restart
			chkconfig crond on
		else
			systemctl restart crond.service
			systemctl enable crond.service
		fi	
fi

if [ ! -f /usr/bin/sar ]; then
	yum -y -q install sysstat
		if [[ "$CENTOS_SEVEN" != '7' ]]; then
			service sysstat restart
			chkconfig sysstat on
		else
			systemctl restart sysstat.service
			systemctl enable sysstat.service
		fi		
fi

if [[ -z "$(crontab -l 2>&1 | grep cminfo_updater)" ]]; then
    crontab -l > cronjoblist
    echo "0 */4 * * * /usr/bin/cminfo_updater" >> cronjoblist
    crontab cronjoblist
    rm -rf cronjoblist
    crontab -l
fi
}