#ifndef __OUT_H
	#define __OUT_H

	class OUT {
		public:
			virtual void SetPin () const = 0;	// Son ctes ya que no cambian objetos dentro de la clase.
			virtual void ClrPin () const = 0;
			virtual void SetDir ( uint8_t direccion ) = 0;
	};

#endif
