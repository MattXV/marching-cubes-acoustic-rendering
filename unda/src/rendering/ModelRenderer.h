#pragma once

#include <iostream>




class ModelRenderer
{
public:
	ModelRenderer();
	~ModelRenderer() = default;

	void drawModel();

private:
	unsigned int programId;
};

