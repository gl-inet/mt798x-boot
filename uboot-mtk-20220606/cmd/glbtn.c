#include <common.h>
#include <command.h>
#include <button.h>
#include <linux/delay.h>
#include <poller.h>
#include <dm/ofnode.h>

static struct poller_async led_p;

int led_control(const char *cmd, const char *name, const char *arg)
{
    const char *led = ofnode_conf_read_str(name);
    char buf[128];

    if (!led)
        return -1;

    sprintf(buf, "%s %s %s", cmd, led, arg);

    run_command(buf, 0);

    return 0;
}

static void gpio_power_clr(void)
{
    ofnode node = ofnode_path("/config");
    char cmd[128];
    const u32 *val;
    int size, i;

    if (!ofnode_valid(node))
        return;

    val = ofnode_read_prop(node, "gpio_power_clr", &size);
    if (!val)
        return;

    for (i = 0; i < size / 4; i++) {
        sprintf(cmd, "gpio clear %u", fdt32_to_cpu(val[i]));
        run_command(cmd, 0);
    }
}

static void gpio_pull_up(void)
{
    ofnode node = ofnode_path("/config");
    char cmd[128];
    const u32 *val;
    int size, i;

    if (!ofnode_valid(node))
        return;

    val = ofnode_read_prop(node, "gpio_pull_up", &size);
    if (!val)
        return;

    for (i = 0; i < size / 4; i++) {
        sprintf(cmd, "gpio set %u", fdt32_to_cpu(val[i]));
        run_command(cmd, 0);
    }
}

static void led_action_post(void *arg)
{
    led_control("ledblink", "blink_led", "0");
    led_control("led", "blink_led", "on");
}

static int do_glbtn(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    const char *button_label = "reset";
    int ret, counter = 0;
    struct udevice *dev;
    ulong ts;

    gpio_power_clr();
    gpio_pull_up();

#ifdef CONFIG_PCIE_TIMING_XE3000
    run_command("gpio set 11", 0);
    mdelay(150);
    run_command("gpio set 10", 0);
    mdelay(200);
    run_command("gpio set 9", 0);
    mdelay(100);
    run_command("gpio clear 10", 0);
#endif

#ifdef CONFIG_PCIE_TIMING_X3000
    run_command("gpio set 10", 0);
    mdelay(200);
    run_command("gpio set 9", 0);
    mdelay(100);
    run_command("gpio clear 10", 0);
#endif

    ret = button_get_by_label(button_label, &dev);
    if (ret) {
        printf("Button '%s' not found (err=%d)\n", button_label, ret);
        return CMD_RET_FAILURE;
    }

    if (!button_get_state(dev)) {
        poller_async_register(&led_p, "led_pa");
        poller_call_async(&led_p, 1000000, led_action_post, NULL);
        return CMD_RET_SUCCESS;
    }

#ifdef CONFIG_LED_GL_XE3000_BLINK
    led_control("ledblink", "internet_led", "500");
#else
    led_control("ledblink", "blink_led", "500");
#endif

    printf("RESET button is pressed for: %2d second(s)", counter++);

    ts = get_timer(0);

    while (button_get_state(dev) && counter < 6) {
        if (get_timer(ts) < 1000)
            continue;

        ts = get_timer(0);

        printf("\b\b\b\b\b\b\b\b\b\b\b\b%2d second(s)", counter++);
    }

    printf("\n");

#ifdef CONFIG_LED_GL_XE3000_BLINK
    led_control("ledblink", "internet_led", "0");
#else
    led_control("ledblink", "blink_led", "0");
#endif

    if (counter == 6) {
#ifdef CONFIG_LED_GL_XE3000_BLINK
        led_control("led", "wifi2g_led", "on");
#else
        led_control("led", "system_led", "on");
#endif
        run_command("httpd", 0);
    } else {
#ifdef CONFIG_LED_GL_XE3000_BLINK
        led_control("ledblink", "internet_led", "0");
#else
        led_control("ledblink", "blink_led", "0");
#endif
    }

    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
    glbtn, 1, 0, do_glbtn,
    "GL-iNet button check",
    ""
);
