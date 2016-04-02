

void meas_trig_init(void);
void meas_trig_unable(void);
void meas_trig_disable(void);

static struct{
	void(*init)(void);
	void(*unable)(void);
	void(*disable)(void);
}osc_meas_trig = {meas_trig_init, meas_trig_unable, meas_trig_disable};

