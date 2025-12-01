#include "FE_pch.h"

#include "GPUDataSerialization.h"

namespace fe
{
	template <typename data_type>
	void EmitVector(YAML::Emitter& emitter, char* dataPtr, Description::Data::Type type)
	{
		emitter << YAML::Flow << YAML::BeginSeq;
		const auto primitive_size = Description::Data::SizeOfPrimitive(Description::Data::PrimitiveInType(type));
		for (size_t i = 0; i < Description::Data::CountInVector(type); ++i)
		{
			emitter << *(data_type*)dataPtr;
			dataPtr += primitive_size;
		}
		emitter << YAML::EndSeq;
	}

	template <typename data_type>
	void EmitMatrix(YAML::Emitter& emitter, char* dataPtr, Description::Data::Type type)
	{
		emitter << YAML::Flow << YAML::BeginSeq;
		const auto primitive_size = Description::Data::SizeOfPrimitive(Description::Data::PrimitiveInType(type));
		for (size_t i = 0; i < Description::Data::RowsOfMatrix(type); ++i)
		{
			emitter << YAML::Flow << YAML::BeginSeq;
			for (size_t j = 0; j < Description::Data::ColumnsOfMatrix(type); ++j)
			{
				emitter << *(data_type*)dataPtr;
				dataPtr += primitive_size;
			}
			emitter << YAML::EndSeq;
		}
		emitter << YAML::EndSeq;
	}

	void EmitGPUDataType(YAML::Emitter& emitter, char* dataPtr, const Description::Data::Type& type)
	{
		auto structure = Description::Data::StructureInType(type);
		auto primitive = Description::Data::PrimitiveInType(type);

		switch (structure)
		{
		case Description::Data::Structure::Scalar:
			switch (primitive)
			{
			case Description::Data::Primitive::Bool:   emitter << *(bool    *)dataPtr; break;
			case Description::Data::Primitive::Int:    emitter << *(int32_t *)dataPtr; break;
			case Description::Data::Primitive::UInt:   emitter << *(uint32_t*)dataPtr; break;
			case Description::Data::Primitive::Float:  emitter << *(float   *)dataPtr; break;
			case Description::Data::Primitive::Double: emitter << *(double  *)dataPtr; break;
			default: FE_CORE_ASSERT(false, "Unrecognized Description::Primitive");
			}
			break;
		case Description::Data::Structure::Vector:
			switch (primitive)
			{
			case Description::Data::Primitive::Bool:   EmitVector<bool    >(emitter, dataPtr, type); break;
			case Description::Data::Primitive::Int:    EmitVector<int     >(emitter, dataPtr, type); break;
			case Description::Data::Primitive::UInt:   EmitVector<uint32_t>(emitter, dataPtr, type); break;
			case Description::Data::Primitive::Float:  EmitVector<float   >(emitter, dataPtr, type); break;
			case Description::Data::Primitive::Double: EmitVector<double  >(emitter, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized Description::Primitive");
			}
			break;
		case Description::Data::Structure::Matrix:
			switch (primitive)
			{
			case Description::Data::Primitive::Bool:   EmitMatrix<bool    >(emitter, dataPtr, type); break;
			case Description::Data::Primitive::Int:    EmitMatrix<int     >(emitter, dataPtr, type); break;
			case Description::Data::Primitive::UInt:   EmitMatrix<uint32_t>(emitter, dataPtr, type); break;
			case Description::Data::Primitive::Float:  EmitMatrix<float   >(emitter, dataPtr, type); break;
			case Description::Data::Primitive::Double: EmitMatrix<double  >(emitter, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized Description::Primitive");
			}
		default: FE_CORE_ASSERT(false, "Unrecognized Description::Structure");
		}	
	}

	template <typename data_type>
	bool LoadVector(const YAML::Node& node, char* dataPtr, Description::Data::Type type)
	{
		size_t vector_size = Description::Data::CountInVector(type);
		size_t primitive_size = Description::Data::SizeOfPrimitive(Description::Data::PrimitiveInType(type));

		if (!node.IsSequence() || node.size() != vector_size) return false;
		for (size_t i = 0; i < vector_size; ++i)
		{
			if constexpr (std::is_same_v<data_type, bool>)
			{
				*(uint32_t*)dataPtr = (uint32_t)node[i].as<bool>();
			}
			else
			{
				*(data_type*)dataPtr = node[i].as<data_type>();
			}
			dataPtr += primitive_size;
		}
		return true;
	}

	template <typename data_type>
	bool LoadMatrix(const YAML::Node& node, char* dataPtr, Description::Data::Type type)
	{
		size_t primitive_size = Description::Data::SizeOfPrimitive(Description::Data::PrimitiveInType(type));
		size_t rows = Description::Data::RowsOfMatrix(type);
		size_t columns = Description::Data::ColumnsOfMatrix(type);

		if (!node.IsSequence() || node.size() != rows) return false;
		for (size_t i = 0; i < rows; ++i)
		{
			const auto& row_node = node[i];
			if (!row_node.IsSequence() || row_node.size() != columns) return false;

			for (size_t j = 0; j < columns; ++j)
			{
				if constexpr (std::is_same_v<data_type, bool>)
				{
					*(uint32_t*)dataPtr = (uint32_t)row_node[j].as<bool>();
				}
				else
				{
					*(data_type*)dataPtr = row_node[j].as<data_type>();
				}
				dataPtr += primitive_size;
			}
		}
		return true;
	}

	bool LoadGPUDataType(const YAML::Node& node, char* dataPtr, Description::Data::Type type)
	{
		FE_PROFILER_FUNC();

		auto structure = Description::Data::StructureInType(type);
		auto primitive = Description::Data::PrimitiveInType(type);
		bool success = false;

		switch (structure)
		{
		case Description::Data::Structure::Scalar:
			switch (primitive)
			{
			case Description::Data::Primitive::Bool:   *(uint32_t*)dataPtr = (uint32_t)node.as<bool>(); success = true; break;
			case Description::Data::Primitive::Int:    *(int*     )dataPtr = node.as<int     >(); success = true; break;
			case Description::Data::Primitive::UInt:   *(uint32_t*)dataPtr = node.as<uint32_t>(); success = true; break;
			case Description::Data::Primitive::Float:  *(float*   )dataPtr = node.as<float   >(); success = true; break;
			case Description::Data::Primitive::Double: *(double*  )dataPtr = node.as<double  >(); success = true; break;
			default: FE_CORE_ASSERT(false, "Unrecognized Description::Primitive");
			}
			break;
		case Description::Data::Structure::Vector:
			switch (primitive)
			{
			case Description::Data::Primitive::Bool:   success = LoadVector<bool    >(node, dataPtr, type); break;
			case Description::Data::Primitive::Int:    success = LoadVector<int     >(node, dataPtr, type); break;
			case Description::Data::Primitive::UInt:   success = LoadVector<uint32_t>(node, dataPtr, type); break;
			case Description::Data::Primitive::Float:  success = LoadVector<float   >(node, dataPtr, type); break;
			case Description::Data::Primitive::Double: success = LoadVector<double  >(node, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized Description::Primitive");
			}
			break;
		case Description::Data::Structure::Matrix:
			switch (primitive)
			{
			case Description::Data::Primitive::Bool:   success = LoadMatrix<bool    >(node, dataPtr, type); break;
			case Description::Data::Primitive::Int:    success = LoadMatrix<int     >(node, dataPtr, type); break;
			case Description::Data::Primitive::UInt:   success = LoadMatrix<uint32_t>(node, dataPtr, type); break;
			case Description::Data::Primitive::Float:  success = LoadMatrix<float   >(node, dataPtr, type); break;
			case Description::Data::Primitive::Double: success = LoadMatrix<double  >(node, dataPtr, type); break;
			default: FE_CORE_ASSERT(false, "Unrecognized Description::Primitive");
			}
		default: FE_CORE_ASSERT(false, "Unrecognized Description::Structure");
		}
		if (!success) return false;

		return true;
	}
}