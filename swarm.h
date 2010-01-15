#ifndef __swarm_h
#define __swarm_h

#include <string>
#include <map>
#include <cuda.h>
#include <cuda_runtime.h>

class integrator;
class gpu_ensemble;
class cpu_ensemble;

//
// Ensemble base class. Note that it has no constructors/destructors/virtuals,
// to allow its instantiation in __constant__ memory on the GPU.
//
class ensemble
{
	protected:
		// number of active (currently integrating) systems, total number of systems
		// and number of bodies per system
		int m_nactive, m_nsys, m_nbod;

		// m_nsys wide array
		float *m_T;

		// m_nsys*m_nbod wide arrays
		double  *m_xyz, *m_vxyz;
		float	*m_m;
		int	*m_active;

		// m_nsys wide array
		int *m_systemIndices;		// map from original systemID to sys index in m_* arrays
		integrator *m_last_integrator;

	public:
		// DEPRECATED: For Young In's code ONLY!!!
		double *xyz()  { return m_xyz; }
		double *vxyz() { return m_vxyz; }
		const double *xyz()  const { return m_xyz; }
		const double *vxyz() const { return m_vxyz; }

	protected:
		void construct_base()
		{
			memset(this, 0, sizeof(*this));
		}

	public:
		// non-const versions
		__host__ __device__ float&   T(int sys) { return m_T[sys]; }
	
		__host__ __device__ double&  x(int sys, int bod) { return m_xyz[bod*m_nsys + sys]; }
		__host__ __device__ double&  y(int sys, int bod) { return m_xyz[m_nbod*m_nsys + bod*m_nsys + sys]; }
		__host__ __device__ double&  z(int sys, int bod) { return m_xyz[m_nbod*m_nsys*2 + bod*m_nsys + sys]; }

		__host__ __device__ double& vx(int sys, int bod) { return m_vxyz[bod*m_nsys + sys]; }
		__host__ __device__ double& vy(int sys, int bod) { return m_vxyz[m_nbod*m_nsys + bod*m_nsys + sys]; }
		__host__ __device__ double& vz(int sys, int bod) { return m_vxyz[m_nbod*m_nsys*2 + bod*m_nsys + sys]; }

		__host__ __device__ float& m(int sys, int bod)   { return m_m[bod*m_nsys + sys]; }

		__host__ __device__ int& active(int sys, int bod){ return m_active[bod*m_nsys + sys]; }

		__host__ __device__ int& nactive() { return m_nactive; }
		__host__ __device__ int& nsys() { return m_nsys; }
		__host__ __device__ int& nbod() { return m_nbod; }

		__host__ __device__ int& index_of_system(int sysId) { return m_systemIndices[sysId]; }


		// const versions
		__host__ __device__ float   T(int sys) const { return m_T[sys]; }
	
		__host__ __device__ double  x(int sys, int bod) const { return m_xyz[bod*m_nsys + sys]; }
		__host__ __device__ double  y(int sys, int bod) const { return m_xyz[m_nbod*m_nsys + bod*m_nsys + sys]; }
		__host__ __device__ double  z(int sys, int bod) const { return m_xyz[m_nbod*m_nsys*2 + bod*m_nsys + sys]; }

		__host__ __device__ double vx(int sys, int bod) const { return m_vxyz[bod*m_nsys + sys]; }
		__host__ __device__ double vy(int sys, int bod) const { return m_vxyz[m_nbod*m_nsys + bod*m_nsys + sys]; }
		__host__ __device__ double vz(int sys, int bod) const { return m_vxyz[m_nbod*m_nsys*2 + bod*m_nsys + sys]; }

		__host__ __device__ float m(int sys, int bod)   const { return m_m[bod*m_nsys + sys]; }

		__host__ __device__ int active(int sys, int bod)const { return m_active[bod*m_nsys + sys]; }

		__host__ __device__ int nactive() const { return m_nactive; }
		__host__ __device__ int nsys() const { return m_nsys; }
		__host__ __device__ int nbod() const { return m_nbod; }

		__host__ __device__ int index_of_system(int sysId) const { return m_systemIndices[sysId]; }



		// convenience
		__host__ __device__ void set_body(int sys, int bod,  float m, double x, double y, double z, double vx, double vy, double vz)
		{
			int idx = bod*m_nsys + sys;

			m_m[idx]   =  m;
			m_xyz[idx] =  x; m_xyz[m_nbod*m_nsys + idx] =  y;  m_xyz[m_nbod*m_nsys*2 + idx] =  z;
			m_vxyz[idx]  = vx;  m_vxyz[m_nbod*m_nsys + idx] = vy;   m_vxyz[m_nbod*m_nsys*2 + idx] = vz;
		}

		__host__ __device__ void get_body(int sys, int bod, float &m, double &x, double &y, double &z, double &vx, double &vy, double &vz) const
		{
			int idx = bod*m_nsys + sys;
			
			 m = m_m[idx];
			 x = m_xyz[idx];   y = m_xyz[m_nbod*m_nsys + idx];   z = m_xyz[m_nbod*m_nsys*2 + idx];
			vx = m_vxyz[idx]; vy = m_vxyz[m_nbod*m_nsys + idx]; vz = m_vxyz[m_nbod*m_nsys*2 + idx];
		}
	public:
		integrator *last_integrator() { return m_last_integrator; }
		void set_last_integrator(integrator *li) { m_last_integrator = li; }

	private:
		friend class cpu_ensemble;
		friend class gpu_ensemble;
};

class cpu_ensemble : public ensemble
{
	public:
		void reset(int nsys, int nbod, bool reinitIndices = true);	// Allocate CPU memory for nsys systems of nbod planets each
		void free();						// Deallocate CPU memory

	public:
		void copy_from(const gpu_ensemble &source);	// Copy the data from the GPU
		
	public:
		cpu_ensemble();					// instantiate an empty ensemble
		cpu_ensemble(int nsys, int nbod);		// instantiate an ensemble with room for nsys systems with nbod each
		cpu_ensemble(const gpu_ensemble &source);	// instantiate a copy of source ensemble

		~cpu_ensemble() { free(); }

	private:
		cpu_ensemble &operator=(const cpu_ensemble&);	// disallow copying
};

class gpu_ensemble : public ensemble
{
	public:
		void reset(int nsys, int nbod, bool reinitIndices = true);	// Allocate GPU memory for nsys systems of nbod planets each
		void free();							// Deallocate CPU memory

	public:
		void copy_from(const cpu_ensemble &source);	// Copy the data from the CPU

	public:
		gpu_ensemble();					// instantiate an empty ensemble
		gpu_ensemble(int nsys, int nbod);		// instantiate an ensemble with room for nsys systems with nbod each
		gpu_ensemble(const cpu_ensemble &source);	// instantiate a copy of the source ensemble

		~gpu_ensemble() { free(); }
};

class integrator
{
	public:
		virtual void integrate(gpu_ensemble &ens, float T) = 0;	// for GPU based integrators
		virtual void integrate(cpu_ensemble &ens, float T) = 0;	// for CPU based integrators

		virtual ~integrator() {};	// has to be here to ensure the derived class' destructor is called (if it exists)
};

// Integrator factory functions (and supporting typedefs)
typedef std::map<std::string, std::string> config;
integrator *create_integrator(const std::string &type, const config &cfg);

// configure grid for nthreads intependent threads, each requiring dynShmemPerThread of shared memory, and
// with each block needing staticShmemPerBlock of shared memory (usually to pass kernel invocation args.)
// The default for staticShmemPerBlock is reasonable (for small kernels), but not necessarily optimal.
bool configure_grid(dim3 &gridDim, int &threadsPerBlock, int nthreads, int dynShmemPerThread = 0, int staticShmemPerBlock = 128);

#endif
