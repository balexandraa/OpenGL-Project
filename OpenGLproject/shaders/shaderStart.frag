#version 410 core

in vec2 passTexture;
in vec3 fNormal; 

in vec4 fragmentPosEyeSpace;  

// spotlight 
in vec4 fragPos; 

out vec4 fragmentColour;


uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform int fog; 

// dir light
uniform int dirLight; 
uniform vec3 lightColor;
uniform vec3 lightDir;

vec3 ambient;
float ambientStrength = 0.4f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;


uniform int bulbEn; 
uniform int bulbMonumentEn; 

uniform int thunderEn; 
uniform float thunderTimer; 

struct Light{
	vec3 position;  
  
    vec3 ambientP;
    vec3 diffuseP;
    vec3 specularP;
	
    float constant;
    float linear;
    float quadratic;
}; 

uniform Light bulbLight[5];  

void computeLightDir() {
    
    if (dirLight == 1) {
        
         //in eye coordinates, the viewer is situated at the origin
	    vec3 cameraPosEye = vec3(0.0f);
	
	    //transform normal
	    vec3 normalEye = normalize(fNormal);	 
	
	    //compute light direction
	    vec3 lightDirN = normalize(lightDir); 
	
	    //compute view direction 
	    vec3 viewDirN = normalize(cameraPosEye - fragmentPosEyeSpace.xyz);
		
	    //compute ambient light
	    ambient = ambientStrength * lightColor;
	
	    //compute diffuse light
	    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	    //compute specular light
	    vec3 reflection = reflect(-lightDirN, normalEye);
	    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	    specular = specularStrength * specCoeff * lightColor;

    } else {
        ambient = vec3(0.0f);
        diffuse = vec3(0.0f);
        specular = vec3(0.0f); 
    }

}

vec3 computeBulbLight(Light bulbLight) {
   float distance    = length(bulbLight.position - fragPos.xyz); 

   float attenuation = 1.0 / (bulbLight.constant + bulbLight.linear * distance + 
    		    bulbLight.quadratic * (distance * distance)); 

   vec3 ambientPoint  = bulbLight.ambientP * attenuation * texture(diffuseTexture, passTexture).rgb; 

   vec3 diffusePoint  = bulbLight.diffuseP*attenuation * texture(diffuseTexture, passTexture).rgb; 

   vec3 specularPoint = bulbLight.specularP * attenuation * texture(diffuseTexture, passTexture).rgb; 
   
   return ambientPoint + diffusePoint + specularPoint; 

}

float computeFog() {

    float fogDensity = 0.05f;
    float fragmentDistance = length(fragmentPosEyeSpace.xyz); 
    float fogFactory = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactory, 0.0f, 1.0f); 
}

void main() {

    computeLightDir();

    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, passTexture).rgb + specular * texture(specularTexture, passTexture).rgb, 1.0f);
    
    // luam culoarea din textura
    vec4 textureColor = texture(diffuseTexture, passTexture);

    vec3 finalColour = color; 

    if (thunderEn == 1) {
       if(thunderTimer < 0.1) {
            finalColour *= 10;
       }
      else {
            finalColour *= 1; 
       }
    }

    if (bulbMonumentEn == 1) {
        finalColour += computeBulbLight(bulbLight[0]); 
    }

    if (bulbEn == 1) {  
        finalColour += computeBulbLight(bulbLight[1]);   
        finalColour += computeBulbLight(bulbLight[2]); 
        finalColour += computeBulbLight(bulbLight[3]); 
        finalColour += computeBulbLight(bulbLight[4]);  
    }
    

    if (fog == 0) {
        // culoarea finala
        fragmentColour = vec4(finalColour, textureColor.a); 
    }
    else {
        float fogFactor = computeFog(); 
        vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

        // culoarea finala
        fragmentColour = mix(fogColor, vec4(finalColour, 1.0f), fogFactor);
    }


}