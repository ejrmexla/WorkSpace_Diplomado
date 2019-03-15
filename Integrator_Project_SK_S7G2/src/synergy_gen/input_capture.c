/* generated thread source file - do not edit */
#include "input_capture.h"

TX_THREAD input_capture;
void input_capture_create(void);
static void input_capture_func(ULONG thread_input);
static uint8_t input_capture_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.input_capture") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if !defined(SSP_SUPPRESS_ISR_g_input_capture) && !defined(SSP_SUPPRESS_ISR_GPT2)
SSP_VECTOR_DEFINE_CHAN(gpt_input_capture_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 2);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_input_capture) && !defined(SSP_SUPPRESS_ISR_GPT2)
SSP_VECTOR_DEFINE_CHAN(gpt_input_capture_isr, GPT, CAPTURE_COMPARE_A, 2);
#endif
static gpt_input_capture_instance_ctrl_t g_input_capture_ctrl;
static const gpt_input_capture_extend_t g_input_capture_extend =
{ .signal = GPT_INPUT_CAPTURE_SIGNAL_PIN_GTIOCA, .signal_filter = GPT_INPUT_CAPTURE_SIGNAL_FILTER_NONE, .clock_divider =
          GPT_INPUT_CAPTURE_CLOCK_DIVIDER_1,
  .enable_level = INPUT_CAPTURE_SIGNAL_LEVEL_NONE, .enable_filter = GPT_INPUT_CAPTURE_SIGNAL_FILTER_NONE, };
static const input_capture_cfg_t g_input_capture_cfg =
{ .channel = 2, .mode = INPUT_CAPTURE_MODE_PERIOD, .edge = INPUT_CAPTURE_SIGNAL_EDGE_FALLING, .repetition =
          INPUT_CAPTURE_REPETITION_PERIODIC,
  .autostart = true, .p_callback = input_capture_callback, .p_context = &g_input_capture, .p_extend =
          &g_input_capture_extend,
  .capture_irq_ipl = (1), .overflow_irq_ipl = (1), };
/* Instance structure to use this module. */
const input_capture_instance_t g_input_capture =
{ .p_ctrl = &g_input_capture_ctrl, .p_cfg = &g_input_capture_cfg, .p_api = &g_input_capture_on_gpt };
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void input_capture_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */

    UINT err;
    err = tx_thread_create (&input_capture, (CHAR *) "InputCapture", input_capture_func, (ULONG) NULL,
                            &input_capture_stack, 1024, 1, 1, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&input_capture, 0);
    }
}

static void input_capture_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */

    /* Enter user code for this thread. */
    input_capture_entry ();
}
