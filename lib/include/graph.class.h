#ifndef DBA_GRAPH_CLASS
#define DBA_GRAPH_CLASS
#include <graphviz/gvc.h>

struct image_type
{
	char* image_buffer = NULL;
	unsigned int image_size = 0;
};

class graph
{
public:

	graph(std::vector<table> *tablelist);
	image_type getImage();
	void write(std::string name, std::string format);
	~graph();

protected:
	GVC_t *gvc;
	Agraph_t *g;
	image_type image;
};
#endif // DBA_GRAPH_CLASS