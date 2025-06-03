#include "FE_pch.h"

#include "ShaderDataSerialization.h"

namespace fe
{
	template <typename data_type>
	void EmitVector(YAML::Emitter& emitter, char* dataPtr, ShaderData::Type type)
	{
		emitter << YAML::Flow << YAML::BeginSeq;
		const auto primitive_size = ShaderData::SizeOfPrimitive(ShaderData::PrimitiveInType(type));
		for (size_t i = 0; i < ShaderData::CountInVector(type); ++i)
		{
			emitter << (data_type)*dataPtr;
			dataPtr += primitive_size;
		}
		emitter << YAML::EndSeq;
	}

	template <typename data_type>
	void EmitMatrix(YAML::Emitter& emitter, char* dataPtr, ShaderData::Type type)
	{
		emitter << YAML::Flow << YAML::BeginSeq;
		const auto primitive_size = ShaderData::SizeOfPrimitive(ShaderData::PrimitiveInType(type));
		for (size_t i = 0; i < ShaderData::RowsOfMatrix(type); ++i)
		{
			emitter << YAML::Flow << YAML::BeginSeq;
			for (size_t j = 0; j < ShaderData::ColumnsOfMatrix(type); ++j)
			{
				emitter << (data_type)*dataPtr;
				dataPtr += primitive_size;
			}
			emitter << YAML::EndSeq;
		}
		emitter << YAML::EndSeq;
	}

	void EmitShaderDataType(YAML::Emitter& emitter, char* dataPtr, const ShaderData::Type& type)
	{
		auto structure = ShaderData::StructureInType(type);
		auto primitive = ShaderData::PrimitiveInType(type);

		switch (structure)
		{
		case ShaderData::Structure::Scalar:
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:   emitter << (bool    )*dataPtr; break;
			case ShaderData::Primitive::Int:    emitter << (int32_t )*dataPtr; break;
			case ShaderData::Primitive::UInt:   emitter << (uint32_t)*dataPtr; break;
			case ShaderData::Primitive::Float:  emitter << (float   )*dataPtr; break;
			case ShaderData::Primitive::Double: emitter << (double  )*dataPtr; break;
			default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
			}
			break;
		case ShaderData::Structure::Vector:
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:   EmitVector<bool    >(emitter, dataPtr, type); break;
			case ShaderData::Primitive::Int:    EmitVector<int     >(emitter, dataPtr, type); break;
			case ShaderData::Primitive::UInt:   EmitVector<uint32_t>(emitter, dataPtr, type); break;
			case ShaderData::Primitive::Float:  EmitVector<float   >(emitter, dataPtr, type); break;
			case ShaderData::Primitive::Double: EmitVector<double  >(emitter, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
			}
			break;
		case ShaderData::Structure::Matrix:
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:   EmitMatrix<bool    >(emitter, dataPtr, type); break;
			case ShaderData::Primitive::Int:    EmitMatrix<int     >(emitter, dataPtr, type); break;
			case ShaderData::Primitive::UInt:   EmitMatrix<uint32_t>(emitter, dataPtr, type); break;
			case ShaderData::Primitive::Float:  EmitMatrix<float   >(emitter, dataPtr, type); break;
			case ShaderData::Primitive::Double: EmitMatrix<double  >(emitter, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
			}
		default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Structure");
		}	
	}

	template <typename data_type>
	bool LoadVector(const YAML::Node& node, char* dataPtr, ShaderData::Type type)
	{
		size_t vector_size = ShaderData::CountInVector(type);
		size_t primitive_size = ShaderData::SizeOfPrimitive(ShaderData::PrimitiveInType(type));

		if (!node.IsSequence() || node.size() != vector_size) return false;
		for (size_t i = 0; i < vector_size; ++i)
		{
			*(data_type*)dataPtr = node[i].as<data_type>();
			dataPtr += primitive_size;
		}
		return true;
	}

	template <typename data_type>
	bool LoadMatrix(const YAML::Node& node, char* dataPtr, ShaderData::Type type)
	{
		size_t primitive_size = ShaderData::SizeOfPrimitive(ShaderData::PrimitiveInType(type));
		size_t rows = ShaderData::RowsOfMatrix(type);
		size_t columns = ShaderData::ColumnsOfMatrix(type);

		if (!node.IsSequence() || node.size() != rows) return false;
		for (size_t i = 0; i < rows; ++i)
		{
			const auto& row_node = node[i];
			if (!row_node.IsSequence() || row_node.size() != columns) return false;

			for (size_t j = 0; j < columns; ++j)
			{
				*(data_type*)dataPtr = row_node[j].as<data_type>();
				dataPtr += primitive_size;
			}
		}
		return true;
	}

	bool LoadShaderDataType(const YAML::Node& node, char* dataPtr, ShaderData::Type type)
	{
		auto structure = ShaderData::StructureInType(type);
		auto primitive = ShaderData::PrimitiveInType(type);
		bool success;

		switch (structure)
		{
		case ShaderData::Structure::Scalar:
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:   *(bool*)dataPtr = node.as<bool    >(); break;
			case ShaderData::Primitive::Int:    *(int*)dataPtr = node.as<int     >(); break;
			case ShaderData::Primitive::UInt:   *(uint32_t*)dataPtr = node.as<uint32_t>(); break;
			case ShaderData::Primitive::Float:  *(float*)dataPtr = node.as<float   >(); break;
			case ShaderData::Primitive::Double: *(double*)dataPtr = node.as<double  >(); break;
			default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
			}
			break;
		case ShaderData::Structure::Vector:
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:   success = LoadVector<bool    >(node, dataPtr, type); break;
			case ShaderData::Primitive::Int:    success = LoadVector<int     >(node, dataPtr, type); break;
			case ShaderData::Primitive::UInt:   success = LoadVector<uint32_t>(node, dataPtr, type); break;
			case ShaderData::Primitive::Float:  success = LoadVector<float   >(node, dataPtr, type); break;
			case ShaderData::Primitive::Double: success = LoadVector<double  >(node, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
			}
			break;
		case ShaderData::Structure::Matrix:
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:   success = LoadMatrix<bool    >(node, dataPtr, type); break;
			case ShaderData::Primitive::Int:    success = LoadMatrix<int     >(node, dataPtr, type); break;
			case ShaderData::Primitive::UInt:   success = LoadMatrix<uint32_t>(node, dataPtr, type); break;
			case ShaderData::Primitive::Float:  success = LoadMatrix<float   >(node, dataPtr, type); break;
			case ShaderData::Primitive::Double: success = LoadMatrix<double  >(node, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
			}
		default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Structure");
		}
		if (!success) return false;

		return true;
	}
}