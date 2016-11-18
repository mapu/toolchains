#ifndef MSPU_INST_FLAGS_H
#define MSPU_INST_FLAGS_H

namespace llvm
{
	namespace MSPU
	{
		namespace SCUFlag
		{
			enum SCUFlag
			{
				CI = 0,
				Flag = 1,
				U = 2, // this order is used for encoding/decoding
				T = 3
			};
		}

		namespace AGUFlag
		{
			enum AGUFlag
			{
				Step   = 0,
				Column = 1,
				U      = 2, // this order is used for encoding/decoding
				AT     = 3
			};
		}
		
		namespace XferFlag
		{
			enum XferFlag {
				U = 0,
				H = 1,
				B = 2
			};
		}

		namespace KMFlag
		{
			enum KMFlag {
				L = 0,
				H = 1
			};
		}

	}

}

#endif // MSPU_INST_FLAGS_H
