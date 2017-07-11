#include <protos.h>
#include <inet.h>
#include <eth.h>
#include <io.h>

static struct protos protos = { .nr_proto = 0, };

struct proto *lookup_proto(uint16_t id)
{
	int i;
	struct proto *p;

	for (i = 0; i < protos.nr_proto; i++) {
		p = &protos.proto[i];
		/*printf("%s: p->id=%X, id=%X\r\n", __func__, p->id, id);*/
		if (p->id == id)
			return p;
	}

	return NULL;
}

int register_proto(int id, struct proto_cb *proto_cb)
{
	struct proto *p;

	if (protos.nr_proto == NR_PROTOS)
		return -1;

	if (!proto_cb || !proto_cb->process)
		return -1;

	p = lookup_proto(id);
	if (p)
		return -1;

	p = &protos.proto[protos.nr_proto++];
	p->id = id;
	p->proto_cb = proto_cb;

	return 0;
}

void protos_init(void)
{
	int i;

	for (i = 0; i < NR_PROTOS; i++)
		memset(&protos.proto[i], 0, sizeof(struct proto));
}
