using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;
using System.Runtime.InteropServices;
using System.IO;

namespace Image_Processor
{

    public class Vec3
    {
        public Vec3(float x = 0, float y = 0, float z = 0)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vec3(Color c)
        {
            this.x = c.R;
            this.y = c.G;
            this.z = c.B;
        }

        public static Vec3 operator+ (Vec3 a, Vec3 b){
            Vec3 v = new Vec3();
            v.x = a.x + b.x;
            v.y = a.y + b.y;
            v.z = a.z + b.z;

            return v;

        }

    public float x, y, z;
    }

    public class Vec2
    {
        public Vec2(float x = 0, float y = 0)
        {
            this.x = x;
            this.y = y;
        }

        public static Vec2 operator +(Vec2 a, Vec2 b)
        {
            Vec2 v = new Vec2();
            v.x = a.x + b.x;
            v.y = a.y + b.y;

            return v;

        }

        public float x, y;
    }

    class Program
    {
        [DllImport(@"../../../../Debug/KernelApplier.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr InitPointer(int x, int y);
        [DllImport(@"../../../../Debug/KernelApplier.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ApplyKernel(string kernel, string kernelApplier);
        //[DllImport(@"../../../../Debug/KernelApplier.dll", CallingConvention = CallingConvention.Cdecl)]
        //public static extern void DeletePointer();
        [DllImport(@"../../../../Debug/KernelApplier.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetValues();
        [DllImport(@"../../../../Debug/KernelApplier.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CleanUp();

        static Vec3 multiplyColour(float i, Color c)
        {
            Vec3 o = new Vec3(c.R * i, c.G * i, c.B * i);

            return o;
        }
        static Color multiplyVec3(float i, Vec3 v)
        {
            Color o = Color.FromArgb(getColor(v.x * i), getColor(v.y * i), getColor(v.z * i));

            return o;
        }
        static int getColor(float i)
        {
            if(i < 0.0f)
            {
                i = 0.0f;
            }else if( i > 255.0f)
            {
                i = 255.0f;
            }
            return (int)i;
        }

        static void filterPixel(int i, int j)
        {
            outputArray[i][j] = multiplyVec3(1.0f,
                multiplyColour(kernelValues[0], inputArray[i - 1][j - 1]) +
                multiplyColour(kernelValues[1], inputArray[i - 1][j]) +
                multiplyColour(kernelValues[2], inputArray[i - 1][j + 1]) +
                multiplyColour(kernelValues[3], inputArray[i][j - 1]) +
                multiplyColour(kernelValues[4], inputArray[i][j]) +
                multiplyColour(kernelValues[5], inputArray[i][j + 1]) +
                multiplyColour(kernelValues[6], inputArray[i + 1][j - 1]) +
                multiplyColour(kernelValues[7], inputArray[i + 1][j]) +
                multiplyColour(kernelValues[8], inputArray[i + 1][j + 1]));
        }

        static string ReadFile(string file)
        {
            string s = "";
            try
            {   // Open the text file using a stream reader.
                using (StreamReader sr = new StreamReader(file))
                {
                    // Read the stream to a string, and write the string to the console.
                    s = sr.ReadToEnd();
                    //Console.WriteLine(line);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("The file could not be read:");
                Console.WriteLine(e.Message);
            }
            return s;
        }

        static float[] GetKernel(string kernel) {
            string kernelStr = ReadFile(kernel);

            float[] kernelValues = new float[9];

            int Index = 0;
            for (int i = 0; i< 9; i++) {

                bool negative = false;
                bool isLhs = true;

                int lhs = 0;
                int rhs = 0;
                int lhsSize = 0;
                int rhsSize = 0;

                do {
                    if (kernelStr[Index] == '-')
                    {
                        negative = true;
                    }
                    else if (kernelStr[Index] >= '0' && kernelStr[Index] <= '9')
                    {
                        if (isLhs)
                        {
                            lhs = lhs * 10 + (kernelStr[Index] - '0');
                            lhsSize++;
                        }
                        else
                        {
                            rhs = rhs * 10 + (kernelStr[Index] - '0');
                            rhsSize++;
                        }
                    }
                    else if (kernelStr[Index] == '.')
                    {
                        isLhs = false;
                    }
                    Index++;
                } while (kernelStr[Index] != ',');

                kernelValues[i] = (float) lhs + ((float) rhs / (float) Math.Pow(10, rhsSize));
			    if (negative) {
				    kernelValues[i] = -kernelValues[i];
			    }
            }
            return kernelValues;
        }

        static Bitmap inputImage;
        static Bitmap outputImage;
        static int height, width;
        static string kernelLocation;
        static float [] kernelValues;

        static Color [][] inputArray;
        static Color [][] outputArray;

        static Thread[] threads;

        static void Main(string[] args)
        {

            System.Console.WriteLine("Insert path of input image:");

            bool inputImageRead = true;
            do {
                inputImageRead = true;
                try
                {
                    string input = "..\\..\\..\\Input Images\\" + System.Console.ReadLine();
                    inputImage = new Bitmap(input);
                }
                catch
                {
                    inputImageRead = false;
                    System.Console.WriteLine("File cannot be read, try again:");
                }
            } while (!inputImageRead);

            System.Console.WriteLine("Insert path of output image:");
            string output = "..\\..\\..\\Output Images\\" + System.Console.ReadLine();

            System.Console.WriteLine("Which Image processing Kernel do you want to use");
            kernelLocation = "..\\..\\..\\Kernels\\" + System.Console.ReadLine();
            bool kernelRead = true;
            do
            {
                kernelRead = true;
                try
                {
                    kernelLocation = "..\\..\\..\\Kernels\\" + System.Console.ReadLine();
                    StreamReader sr = new StreamReader(kernelLocation);
                    sr.Close();
                }
                catch
                {
                    kernelRead = false;
                    System.Console.WriteLine("File cannot be read, try again:");
                }
            } while (!kernelRead);

            System.Console.WriteLine("Choose image Analysis Method (0, loop), (1, threads), (2, GPU):");
            int method = Convert.ToInt32(System.Console.ReadLine());

            height = inputImage.Height; width = inputImage.Width;

            outputImage = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format32bppArgb );

            switch (method)
            {
                case 0:
                    {
                        kernelValues = GetKernel(kernelLocation);
                        inputArray = new Color[width + 2][];
                        outputArray = new Color[width + 2][];


                        for (int i = 1; i < width + 1; i++)
                        {
                            inputArray[i] = new Color[height + 2];
                            outputArray[i] = new Color[height + 2];

                            for (int j = 1; j < height + 1; j++)
                            {

                                inputArray[i][j] = inputImage.GetPixel(i - 1, j - 1);

                            }
                        }

                        for (int i = 1; i < width + 1; i++)
                        {
                            inputArray[i][0] = inputArray[i][1];
                            inputArray[i][height + 1] = inputArray[i][height];
                        }
                        inputArray[0] = new Color[height + 2];
                        inputArray[width + 1] = new Color[height + 2];
                        for (int j = 0; j < height + 2; j++)
                        {
                            inputArray[0][j] = inputArray[1][j];
                            inputArray[width + 1][j] = inputArray[width][j];
                        }

                        //threads = new Thread[8];

                        for (int i = 0; i < 8; i++)
                        {

                            int tempi = i;

                            //threads[i] = new Thread(() =>
                            {
                                for (int j = width * tempi / 8 + 1; j < width * (tempi + 1) / 8 + 1; j++)
                                {
                                    System.Console.WriteLine(j);
                                    for (int k = 1; k < height + 1; k++)
                                    {
                                        filterPixel(j, k);
                                    }
                                }
                                System.Console.WriteLine(tempi + "is completed");
                            }
                        }

                        for (int i = 0; i < width; i++)
                        {

                            for (int j = 0; j < height; j++)
                            {
                                outputImage.SetPixel(i, j, outputArray[i + 1][j + 1]);
                            }
                        }
                        outputImage.Save(output, ImageFormat.Jpeg);
                    }
                    break;
                case 1:
                    {
                        kernelValues = GetKernel(kernelLocation);
                        inputArray = new Color[width + 2][];
                        outputArray = new Color[width + 2][];


                        for (int i = 1; i < width + 1; i++)
                        {
                            inputArray[i] = new Color[height + 2];
                            outputArray[i] = new Color[height + 2];

                            for (int j = 1; j < height + 1; j++)
                            {

                                inputArray[i][j] = inputImage.GetPixel(i - 1, j - 1);

                            }
                        }

                        for (int i = 1; i < width + 1; i++)
                        {
                            inputArray[i][0] = inputArray[i][1];
                            inputArray[i][height + 1] = inputArray[i][height];
                        }
                        inputArray[0] = new Color[height + 2];
                        inputArray[width + 1] = new Color[height + 2];
                        for (int j = 0; j < height + 2; j++)
                        {
                            inputArray[0][j] = inputArray[1][j];
                            inputArray[width + 1][j] = inputArray[width][j];
                        }

                        threads = new Thread[8];

                        for (int i = 0; i < 8; i++)
                        {

                            int tempi = i;

                            threads[i] = new Thread(() =>
                            {
                                for (int j = width * tempi / 8 + 1; j < width * (tempi + 1) / 8 + 1; j++)
                                {
                                    System.Console.WriteLine(j);
                                    for (int k = 1; k < height + 1; k++)
                                    {
                                        filterPixel(j, k);
                                    }
                                }
                                System.Console.WriteLine(tempi + "is completed");
                            }
                            );
                            threads[i].Start();
                        }

                        for (int i = 0; i < 8; i++)
                        {
                            threads[i].Join();
                        }

                        for (int i = 0; i < width; i++)
                        {

                            for (int j = 0; j < height; j++)
                            {
                                outputImage.SetPixel(i, j, outputArray[i + 1][j + 1]);
                            }
                        }
                        outputImage.Save(output, ImageFormat.Jpeg);
                    }
                    break;
                case 2:
                    {
                        int[] inputIntArray = new int[width * height * 4];


                        for (int i = 0; i < width * height; i++)
                        {
                            inputIntArray[i * 4] = inputImage.GetPixel(i % width, i / width).R;
                            inputIntArray[i * 4 + 1] = inputImage.GetPixel(i % width, i / width).G;
                            inputIntArray[i * 4 + 2] = inputImage.GetPixel(i % width, i / width).B;

                        }

                        IntPtr ptr = InitPointer(width, height);

                        Marshal.Copy(inputIntArray, 0, ptr, width * height * 4);

                        ApplyKernel(kernelLocation, "..\\..\\..\\..\\KernelApplier\\KernelApplier.cl");

                        //DeletePointer();

                        ptr = GetValues();

                        Marshal.Copy(ptr, inputIntArray, 0, width * height * 4);

                        CleanUp();

                        for (int i = 0; i < width; i++)
                        {

                            for (int j = 0; j < height; j++)
                            {
                                Color c = Color.FromArgb(inputIntArray[(j * width + i) * 4], inputIntArray[(j * width + i) * 4 + 1], inputIntArray[(j * width + i) * 4 + 2]);
                                outputImage.SetPixel(i, j, c);
                            }
                        }
                        outputImage.Save(output, ImageFormat.Jpeg);
                    }
                    break;
            }

            return;
        }
    }
}
