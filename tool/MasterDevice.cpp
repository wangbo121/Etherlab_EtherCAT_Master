/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sstream>
#include <iomanip>
using namespace std;

#include "MasterDevice.h"

/****************************************************************************/

MasterDevice::MasterDevice()
{
    index = 0;
    fd = -1;
}

/****************************************************************************/

MasterDevice::~MasterDevice()
{
    close();
}

/****************************************************************************/

void MasterDevice::setIndex(unsigned int i)
{
    index = i;
}

/****************************************************************************/

void MasterDevice::open(Permissions perm)
{
    stringstream deviceName;

    if (fd == -1) { // not already open
		deviceName << "/dev/EtherCAT" << index;

		if ((fd = ::open(deviceName.str().c_str(),
						perm == ReadWrite ? O_RDWR : O_RDONLY)) == -1) {
			stringstream err;
			err << "Failed to open master device " << deviceName.str() << ": "
				<< strerror(errno);
			throw MasterDeviceException(err.str());
		}
    }
}

/****************************************************************************/

void MasterDevice::close()
{
    if (fd != -1) {
		::close(fd);
		fd = -1;
	}
}

/****************************************************************************/

unsigned int MasterDevice::slaveCount()
{
    ec_ioctl_master_t data;

    getMaster(&data);
    return data.slave_count;
}

/****************************************************************************/

void MasterDevice::getMaster(ec_ioctl_master_t *data)
{
    if (ioctl(fd, EC_IOCTL_MASTER, data) < 0) {
        stringstream err;
        err << "Failed to get master information: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getConfig(ec_ioctl_config_t *data, unsigned int index)
{
    data->config_index = index;

    if (ioctl(fd, EC_IOCTL_CONFIG, data) < 0) {
        stringstream err;
        err << "Failed to get slave configuration: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getConfigPdo(
        ec_ioctl_config_pdo_t *data,
        unsigned int index,
        uint8_t sync_index,
        uint16_t pdo_pos
        )
{
    data->config_index = index;
    data->sync_index = sync_index;
    data->pdo_pos = pdo_pos;

    if (ioctl(fd, EC_IOCTL_CONFIG_PDO, data) < 0) {
        stringstream err;
        err << "Failed to get slave config Pdo: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getConfigPdoEntry(
        ec_ioctl_config_pdo_entry_t *data,
        unsigned int index,
        uint8_t sync_index,
        uint16_t pdo_pos,
        uint8_t entry_pos
        )
{
    data->config_index = index;
    data->sync_index = sync_index;
    data->pdo_pos = pdo_pos;
    data->entry_pos = entry_pos;

    if (ioctl(fd, EC_IOCTL_CONFIG_PDO_ENTRY, data) < 0) {
        stringstream err;
        err << "Failed to get slave config Pdo entry: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getConfigSdo(
        ec_ioctl_config_sdo_t *data,
        unsigned int index,
        unsigned int sdo_pos
        )
{
    data->config_index = index;
    data->sdo_pos = sdo_pos;

    if (ioctl(fd, EC_IOCTL_CONFIG_SDO, data) < 0) {
        stringstream err;
        err << "Failed to get slave config Sdo: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getDomain(ec_ioctl_domain_t *data, unsigned int index)
{
    data->index = index;

    if (ioctl(fd, EC_IOCTL_DOMAIN, data)) {
        stringstream err;
        err << "Failed to get domain: ";
        if (errno == EINVAL)
            err << "Domain " << index << " does not exist!";
        else
            err << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getData(ec_ioctl_domain_data_t *data,
        unsigned int domainIndex, unsigned int dataSize, unsigned char *mem)
{
    data->domain_index = domainIndex;
    data->data_size = dataSize;
    data->target = mem;

    if (ioctl(fd, EC_IOCTL_DOMAIN_DATA, data) < 0) {
        stringstream err;
        err << "Failed to get domain data: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getSlave(ec_ioctl_slave_t *slave, uint16_t slaveIndex)
{
    slave->position = slaveIndex;

    if (ioctl(fd, EC_IOCTL_SLAVE, slave)) {
        stringstream err;
        err << "Failed to get slave: ";
        if (errno == EINVAL)
            err << "Slave " << slaveIndex << " does not exist!";
        else
            err << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getFmmu(
        ec_ioctl_domain_fmmu_t *fmmu,
        unsigned int domainIndex,
        unsigned int fmmuIndex
        )
{
    fmmu->domain_index = domainIndex;
    fmmu->fmmu_index = fmmuIndex;

    if (ioctl(fd, EC_IOCTL_DOMAIN_FMMU, fmmu)) {
        stringstream err;
        err << "Failed to get domain FMMU: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getSync(
        ec_ioctl_slave_sync_t *sync,
        uint16_t slaveIndex,
        uint8_t syncIndex
        )
{
    sync->slave_position = slaveIndex;
    sync->sync_index = syncIndex;

    if (ioctl(fd, EC_IOCTL_SLAVE_SYNC, sync)) {
        stringstream err;
        err << "Failed to get sync manager: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getPdo(
        ec_ioctl_slave_sync_pdo_t *pdo,
        uint16_t slaveIndex,
        uint8_t syncIndex,
        uint8_t pdoPos
        )
{
    pdo->slave_position = slaveIndex;
    pdo->sync_index = syncIndex;
    pdo->pdo_pos = pdoPos;

    if (ioctl(fd, EC_IOCTL_SLAVE_SYNC_PDO, pdo)) {
        stringstream err;
        err << "Failed to get Pdo: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getPdoEntry(
        ec_ioctl_slave_sync_pdo_entry_t *entry,
        uint16_t slaveIndex,
        uint8_t syncIndex,
        uint8_t pdoPos,
        uint8_t entryPos
        )
{
    entry->slave_position = slaveIndex;
    entry->sync_index = syncIndex;
    entry->pdo_pos = pdoPos;
    entry->entry_pos = entryPos;

    if (ioctl(fd, EC_IOCTL_SLAVE_SYNC_PDO_ENTRY, entry)) {
        stringstream err;
        err << "Failed to get Pdo entry: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getSdo(
        ec_ioctl_slave_sdo_t *sdo,
        uint16_t slaveIndex,
        uint16_t sdoPosition
        )
{
    sdo->slave_position = slaveIndex;
    sdo->sdo_position = sdoPosition;

    if (ioctl(fd, EC_IOCTL_SLAVE_SDO, sdo)) {
        stringstream err;
        err << "Failed to get Sdo: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::getSdoEntry(
        ec_ioctl_slave_sdo_entry_t *entry,
        uint16_t slaveIndex,
        int sdoSpec,
        uint8_t entrySubindex
        )
{
    entry->slave_position = slaveIndex;
    entry->sdo_spec = sdoSpec;
    entry->sdo_entry_subindex = entrySubindex;

    if (ioctl(fd, EC_IOCTL_SLAVE_SDO_ENTRY, entry)) {
        stringstream err;
        err << "Failed to get Sdo entry: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::readSii(
        ec_ioctl_slave_sii_t *data
        )
{
    if (ioctl(fd, EC_IOCTL_SLAVE_SII_READ, data) < 0) {
        stringstream err;
        err << "Failed to read SII: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::writeSii(
        ec_ioctl_slave_sii_t *data
        )
{
    if (ioctl(fd, EC_IOCTL_SLAVE_SII_WRITE, data) < 0) {
        stringstream err;
        err << "Failed to write SII: " << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::setDebug(unsigned int debugLevel)
{
    if (ioctl(fd, EC_IOCTL_MASTER_DEBUG, debugLevel) < 0) {
        stringstream err;
        err << "Failed to set debug level: " << strerror(errno);
        throw MasterDeviceException(err.str());
	}
}

/****************************************************************************/

void MasterDevice::sdoDownload(ec_ioctl_slave_sdo_download_t *data)
{
    if (ioctl(fd, EC_IOCTL_SLAVE_SDO_DOWNLOAD, data) < 0) {
        stringstream err;
        err << "Failed to download Sdo: ";
        if (errno == EIO && data->abort_code) {
            err << "Abort code 0x" << hex << setfill('0')
                << setw(8) << data->abort_code;
        } else {
            err << strerror(errno);
        }
        throw MasterDeviceException(err.str());
	}
}

/****************************************************************************/

void MasterDevice::sdoUpload(ec_ioctl_slave_sdo_upload_t *data)
{
    if (ioctl(fd, EC_IOCTL_SLAVE_SDO_UPLOAD, &data) < 0) {
        stringstream err;
        err << "Failed to upload Sdo: ";
        if (errno == EIO && data->abort_code) {
            err << "Abort code 0x" << hex << setfill('0')
                << setw(8) << data->abort_code;
        } else {
            err << strerror(errno);
        }
        throw MasterDeviceException(err.str());
    }
}

/****************************************************************************/

void MasterDevice::requestState(
        uint16_t slavePosition,
        uint8_t state
        )
{
    ec_ioctl_slave_state_t data;

    data.slave_position = slavePosition;
    data.requested_state = state;
    
    if (ioctl(fd, EC_IOCTL_SLAVE_STATE, &data)) {
        stringstream err;
        err << "Failed to request slave state: ";
        if (errno == EINVAL)
            err << "Slave " << slavePosition << " does not exist!";
        else
            err << strerror(errno);
        throw MasterDeviceException(err.str());
    }
}

/*****************************************************************************/