#include <common.h>
#include <phy.h>

static int gpy211_probe(struct phy_device *phydev)
{
	phydev->flags |= PHY_FLAG_BROKEN_RESET;
	return 0;
}

static int gpy211_config(struct phy_device *phydev)
{
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 8, 0xa4fa);
	return 0;
}

static int gpy211_startup(struct phy_device *phydev)
{
	int ret;

	ret = genphy_update_link(phydev);
	if (ret)
		return ret;

	phydev->speed = SPEED_1000;
	phydev->duplex = DUPLEX_FULL;

	return 0;
}

static struct phy_driver GPY211_driver = {
	.name = "Intel GPY211 10/100/1000/2500Mbps Ethernet",
	.uid = 0x67c9de10,
	.mask = 0xffffff,
	.features = PHY_GBIT_FEATURES,
	.probe = &gpy211_probe,
	.config = &gpy211_config,
	.startup = &gpy211_startup,
	.shutdown = &genphy_shutdown,
};

int phy_gpy211_init(void)
{
	phy_register(&GPY211_driver);

	return 0;
}
